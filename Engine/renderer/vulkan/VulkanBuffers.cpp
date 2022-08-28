
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createVertexBuffer(FMesh* pMesh) {
  UTRACE("Creating vertex buffer...");

  mVertexBufferTriangle.vertexCount = pMesh->vertices.size();

  VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.size = sizeof(pMesh->vertices[0]) * pMesh->vertices.size();
  createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 0;
  createInfo.pQueueFamilyIndices = nullptr;

  VkResult created{ vkCreateBuffer(mVkDevice, &createInfo, nullptr, &mVertexBufferTriangle.handle) };
  if (created != VK_SUCCESS) {
    UERROR("Could not create vertex buffer!");
    return UFALSE;
  }

  VkMemoryRequirements memoryReqs;
  vkGetBufferMemoryRequirements(mVkDevice, mVertexBufferTriangle.handle, &memoryReqs);

  VkPhysicalDeviceMemoryProperties memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &memoryProperties);

  VkMemoryPropertyFlagBits propertyFlags{ (VkMemoryPropertyFlagBits)
    (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };
  u32 memoryTypeIndex{ findMemoryIndex(memoryProperties, memoryReqs.memoryTypeBits,
                                       propertyFlags) };

  if (memoryTypeIndex == UUNUSED) {
    UERROR("Required memory type index not found, vertex buffer is not valid!");
    return UFALSE;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = memoryReqs.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  VkResult allocated{ vkAllocateMemory(mVkDevice, &memoryAllocateInfo, nullptr,
                                       &mVertexBufferTriangle.deviceMemory) };
  if (allocated != VK_SUCCESS) {
    UERROR("Could not allocate device memory for vertex buffer!");
    return UFALSE;
  }

  VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(mVkDevice, mVertexBufferTriangle.handle, mVertexBufferTriangle.deviceMemory,
                     memoryOffset);

  void* pBufferData{ nullptr };
  vkMapMemory(mVkDevice, mVertexBufferTriangle.deviceMemory, 0, createInfo.size, 0, &pBufferData);
  memcpy(pBufferData, pMesh->vertices.data(), createInfo.size);
  vkUnmapMemory(mVkDevice, mVertexBufferTriangle.deviceMemory);

  UDEBUG("Created vertex buffer!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeVertexBuffer() {
  UTRACE("Closing vertex buffer...");

  if (mVertexBufferTriangle.handle != VK_NULL_HANDLE) {
    UTRACE("Destroying vertex buffer...");
    vkDestroyBuffer(mVkDevice, mVertexBufferTriangle.handle, nullptr);
    mVertexBufferTriangle.handle = VK_NULL_HANDLE;
  }
  else {
    UWARN("As vertex buffer is not created, it is not destroyed!");
  }

  if (mVertexBufferTriangle.deviceMemory != VK_NULL_HANDLE) {
    UTRACE("Freeing vertex buffer memory...");
    vkFreeMemory(mVkDevice, mVertexBufferTriangle.deviceMemory, nullptr);
    mVertexBufferTriangle.deviceMemory = VK_NULL_HANDLE;
  }
  else {
    UWARN("As vertex buffer is not allocated, it won't be freed!");
  }

  mVertexBufferTriangle.vertexCount = 0;

  UDEBUG("Closed vertex buffer!");
  return UTRUE;
}


}
