
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


static b32 createBuffer(VkDevice device, VkBufferUsageFlags  usage, VkDeviceSize size,
                        VkBuffer* pOutHandle, const char* logInfo);


static b32 allocateAndBindBufferMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                                       VkBuffer bufferHandle,
                                       VkMemoryPropertyFlags memoryPropertyFlags,
                                       VkDeviceMemory* pOutDeviceMemoryHandle,
                                       const char* logInfo);


template<typename T>
static b32 copyDataFromHostToBuffer(VkDevice device, VkDeviceMemory deviceMemory,
                                    VkDeviceSize size, T data, const char* logInfo);


static b32 copyDataFromDeviceBufferToBuffer(VkDevice device, VkCommandPool transferCommandPool,
                                            VkQueue transferQueue, VkDeviceSize size,
                                            VkBuffer srcBuffer, VkBuffer dstBuffer,
                                            const char* srcLogInfo, const char* dstLogInfo);


template<typename T>
static b32 closeBuffer(VkDevice device, T pBuffer, const char* logInfo);


template<VkBufferUsageFlags TUsage, typename TData, typename TBuffer>
static b32 createAllocateAndCopyMeshDataToVertexBuffer(
    VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool transferCommandPool,
    VkQueue transferQueue, VkDeviceSize bufferSize, TData pData, TBuffer pOutBuffer,
    const char* logInfo);


b32 FRenderContextVulkan::createBuffersForMesh(FMesh* pMesh, FVertexBufferVulkan* pOutVertex,
                                               FIndexBufferVulkan* pOutIndex) {
  UTRACE("Creating buffers for mesh...");

  pOutVertex->vertexCount = pMesh->vertices.size();
  VkDeviceSize vertexBufferSize{ sizeof(pMesh->vertices[0]) * pMesh->vertices.size() };

  createAllocateAndCopyMeshDataToVertexBuffer<VK_BUFFER_USAGE_VERTEX_BUFFER_BIT>(
      mVkPhysicalDevice, mVkDevice, mVkTransferCommandPool,
      mVkTransferQueueVector[mCopyQueueIndex], vertexBufferSize, pMesh->vertices.data(),
      pOutVertex, "vertex");
  UDEBUG("Created vertex buffer for mesh!");

  pOutIndex->indicesCount = pMesh->indices.size();
  VkDeviceSize indicesSize{ sizeof(pMesh->indices[0]) * pMesh->indices.size() };

  createAllocateAndCopyMeshDataToVertexBuffer<VK_BUFFER_USAGE_INDEX_BUFFER_BIT>(
      mVkPhysicalDevice, mVkDevice, mVkTransferCommandPool,
      mVkTransferQueueVector[mCopyQueueIndex], indicesSize, pMesh->indices.data(),
      pOutIndex, "index");
  UDEBUG("Created index buffer for mesh!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeBuffersForMesh(FVertexBufferVulkan* pVertex,
                                              FIndexBufferVulkan* pIndex) {
  UTRACE("Closing buffers for mesh...");

  closeBuffer(mVkDevice, pVertex, "vertex");
  pVertex->vertexCount = 0;

  closeBuffer(mVkDevice, pIndex, "index");
  pIndex->indicesCount = 0;

  UDEBUG("Closed buffers for mesh!");
  return UTRUE;
}


b32 createBuffer(VkDevice device, VkBufferUsageFlags  usage, VkDeviceSize size,
                 VkBuffer* pOutHandle, const char* logInfo) {
  UTRACE("Creating {} buffer...", logInfo);

  VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.size = size;
  createInfo.usage = usage;
  createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 0;
  createInfo.pQueueFamilyIndices = nullptr;

  VkResult created{ vkCreateBuffer(device, &createInfo, nullptr, pOutHandle) };
  if (created != VK_SUCCESS) {
    UERROR("Could not create {} buffer!", logInfo);
    return UFALSE;
  }

  UTRACE("Created {} buffer!", logInfo);
  return UTRUE;
}


b32 allocateAndBindBufferMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                                VkBuffer bufferHandle, VkMemoryPropertyFlags memoryPropertyFlags,
                                VkDeviceMemory* pOutDeviceMemoryHandle, const char* logInfo) {
  UTRACE("Allocating and binding {} buffer memory...", logInfo);

  VkMemoryRequirements memoryReqs;
  vkGetBufferMemoryRequirements(device, bufferHandle, &memoryReqs);

  u32 memoryTypeIndex{ findMemoryIndex(physicalDevice, memoryReqs.memoryTypeBits,
                                       memoryPropertyFlags) };

  if (memoryTypeIndex == UUNUSED) {
    UERROR("Required memory type index not found, {} buffer is not valid!", logInfo);
    return UFALSE;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = memoryReqs.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  VkResult allocated{ vkAllocateMemory(device, &memoryAllocateInfo, nullptr,
                                       pOutDeviceMemoryHandle) };
  if (allocated != VK_SUCCESS) {
    UERROR("Could not allocate device memory for {} buffer!", logInfo);
    return UFALSE;
  }

  VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(device, bufferHandle, *pOutDeviceMemoryHandle, memoryOffset);

  UTRACE("Allocated and bound {} buffer memory!", logInfo);
  return UTRUE;
}


template<typename T>
b32 copyDataFromHostToBuffer(VkDevice device, VkDeviceMemory deviceMemory,
                             VkDeviceSize size, T data, const char* logInfo) {
  UTRACE("Copying {} of {} data to {} buffer...", size, typeid(T).name(), logInfo);

  void* pBufferData{ nullptr };
  vkMapMemory(device, deviceMemory, 0, size, 0, &pBufferData);
  memcpy(pBufferData, data, size);
  vkUnmapMemory(device, deviceMemory);

  UTRACE("Copied {} of {} data to {} buffer!", size, typeid(T).name(), logInfo);
  return UTRUE;
}


b32 copyDataFromDeviceBufferToBuffer(VkDevice device, VkCommandPool transferCommandPool,
                                     VkQueue transferQueue, VkDeviceSize size,
                                     VkBuffer srcBuffer, VkBuffer dstBuffer,
                                     const char* srcLogInfo, const char* dstLogInfo) {
  UTRACE("Copying data from {} buffer to {} buffer...", srcLogInfo, dstLogInfo);

  VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandPool = transferCommandPool;
  allocateInfo.commandBufferCount = 1;

  VkCommandBuffer transferCommandBuffer{ VK_NULL_HANDLE };
  U_VK_ASSERT( vkAllocateCommandBuffers(device, &allocateInfo, &transferCommandBuffer) );

  VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  beginInfo.pNext = nullptr;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  U_VK_ASSERT( vkBeginCommandBuffer(transferCommandBuffer, &beginInfo) );

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0; // Optional
  copyRegion.dstOffset = 0; // Optional
  copyRegion.size = size;

  vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  U_VK_ASSERT( vkEndCommandBuffer(transferCommandBuffer) );

  VkSubmitInfo copySubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  copySubmitInfo.pNext = nullptr;
  copySubmitInfo.waitSemaphoreCount = 0;
  copySubmitInfo.pWaitSemaphores = nullptr;
  copySubmitInfo.pWaitDstStageMask = nullptr;
  copySubmitInfo.commandBufferCount = 1;
  copySubmitInfo.pCommandBuffers = &transferCommandBuffer;
  copySubmitInfo.signalSemaphoreCount = 0;
  copySubmitInfo.pSignalSemaphores = nullptr;

  U_VK_ASSERT( vkQueueSubmit(transferQueue, 1, &copySubmitInfo, VK_NULL_HANDLE) );
  vkQueueWaitIdle(transferQueue);

  vkFreeCommandBuffers(device, transferCommandPool, 1, &transferCommandBuffer);

  UTRACE("Copied data from {} buffer to {} buffer!", srcLogInfo, dstLogInfo);
  return UTRUE;
}


template<VkBufferUsageFlags TUsage, typename TData, typename TBuffer>
b32 createAllocateAndCopyMeshDataToVertexBuffer(
    VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool transferCommandPool,
    VkQueue transferQueue, VkDeviceSize bufferSize, TData pData, TBuffer pOutBuffer,
    const char* logInfo) {
  FVertexBufferVulkan stagingBuffer{};
  VkMemoryPropertyFlags stagingPropertyFlags{
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
  createBuffer(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, bufferSize, &stagingBuffer.handle,
               "staging");
  allocateAndBindBufferMemory(physicalDevice, device, stagingBuffer.handle, stagingPropertyFlags,
                              &stagingBuffer.deviceMemory, "staging");
  copyDataFromHostToBuffer(device, stagingBuffer.deviceMemory, bufferSize, pData, "staging");

  VkBufferUsageFlags actualBufferUsage{ VK_BUFFER_USAGE_TRANSFER_DST_BIT | TUsage };
  VkMemoryPropertyFlags actualMemoryPropertyFlags{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
  createBuffer(device, actualBufferUsage, bufferSize, &pOutBuffer->handle, logInfo);
  allocateAndBindBufferMemory(physicalDevice, device, pOutBuffer->handle,
                              actualMemoryPropertyFlags, &pOutBuffer->deviceMemory,
                              logInfo);
  copyDataFromDeviceBufferToBuffer(device, transferCommandPool, transferQueue, bufferSize,
                                   stagingBuffer.handle, pOutBuffer->handle,
                                   "host visible staging", logInfo);

  closeBuffer(device, &stagingBuffer, "staging");

  return UTRUE;
}


template<typename T>
b32 closeBuffer(VkDevice device, T pBuffer, const char* logInfo) {
  if (pBuffer->handle != VK_NULL_HANDLE) {
    UTRACE("Destroying {} buffer...", logInfo);
    vkDestroyBuffer(device, pBuffer->handle, nullptr);
    pBuffer->handle = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} buffer is not created, it is not destroyed!", logInfo);
  }

  if (pBuffer->deviceMemory != VK_NULL_HANDLE) {
    UTRACE("Freeing {} buffer memory...", logInfo);
    vkFreeMemory(device, pBuffer->deviceMemory, nullptr);
    pBuffer->deviceMemory = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} buffer is not allocated, it won't be freed!", logInfo);
  }

  return UTRUE;
}


}
