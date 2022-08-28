
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


b32 FRenderContextVulkan::createVertexBuffer(FMesh* pMesh) {
  UTRACE("Creating vertex buffer...");

  mVertexBufferTriangle.vertexCount = pMesh->vertices.size();
  VkDeviceSize bufferSize{ sizeof(pMesh->vertices[0]) * pMesh->vertices.size() };

  FVertexBufferVulkan stagingVertexBuffer{};
  VkMemoryPropertyFlags stagingPropertyFlags{
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
  createBuffer(mVkDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, bufferSize,
               &stagingVertexBuffer.handle, "staging vertex");
  allocateAndBindBufferMemory(mVkPhysicalDevice, mVkDevice, stagingVertexBuffer.handle,
                              stagingPropertyFlags, &stagingVertexBuffer.deviceMemory,
                              "staging vertex");
  copyDataFromHostToBuffer(mVkDevice, stagingVertexBuffer.deviceMemory, bufferSize,
                           pMesh->vertices.data(), "staging vertex");

  VkBufferUsageFlags actualVertexBufferUsage{
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT };
  VkMemoryPropertyFlags actualVertexMemoryPropertyFlags{
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
  createBuffer(mVkDevice, actualVertexBufferUsage, bufferSize,
               &mVertexBufferTriangle.handle, "actual vertex");
  allocateAndBindBufferMemory(mVkPhysicalDevice, mVkDevice, mVertexBufferTriangle.handle,
                              actualVertexMemoryPropertyFlags, &mVertexBufferTriangle.deviceMemory,
                              "actual vertex");
  copyDataFromDeviceBufferToBuffer(mVkDevice, mVkTransferCommandPool,
                                   mVkTransferQueueVector[mCopyQueueIndex], bufferSize,
                                   stagingVertexBuffer.handle, mVertexBufferTriangle.handle,
                                   "host visible staging", "actual vertex");

  closeBuffer(mVkDevice, &stagingVertexBuffer, "staging vertex");

  UDEBUG("Created vertex buffer!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeVertexBuffer() {
  UTRACE("Closing vertex buffer...");

  closeBuffer(mVkDevice, &mVertexBufferTriangle, "vertex");
  mVertexBufferTriangle.vertexCount = 0;

  UDEBUG("Closed vertex buffer!");
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


template<typename T>
static b32 closeBuffer(VkDevice device, T pBuffer, const char* logInfo) {
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
