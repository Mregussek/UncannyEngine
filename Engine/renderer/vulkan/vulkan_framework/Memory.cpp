
#include "Memory.h"
#include "Utilities.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


u32 FMemoryVulkan::sNumOfAllocations{ 0 };


static b32 copyDataFromHostToBuffer(VkDevice device, VkDeviceMemory deviceMemory,
                                    VkDeviceSize size, void* pData, const char* logInfo);


static b32 copyDataFromDeviceToBuffer(
    VkDevice device, VkDeviceSize size,
    const FMemoryCopyBuffersUsingStagingBufferDependenciesVulkan* pDeps);


b32 FMemoryVulkan::allocate(const FMemoryAllocationDependenciesVulkan& deps,
                            VkDeviceMemory* pOutDeviceMemoryHandle) {
  UTRACE("Allocating {} memory...", deps.logInfo);

  u32 memoryTypeIndex{ UUNUSED };
  b32 foundIndex{ FMemoryVulkan::findMemoryIndex(deps.physicalDevice,
                                                 deps.memoryRequirements.memoryTypeBits,
                                                 deps.propertyFlags, &memoryTypeIndex) };
  if (foundIndex == UUNUSED) {
    UERROR("Required memory type index not found, {} is not valid!", deps.logInfo);
    return UFALSE;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = deps.memoryRequirements.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  VkResult allocated{ vkAllocateMemory(deps.device, &memoryAllocateInfo, nullptr,
                                       pOutDeviceMemoryHandle) };
  if (allocated != VK_SUCCESS) {
    UERROR("Could not allocate device memory for {}!", deps.logInfo);
    return UFALSE;
  }
  sNumOfAllocations++;

  UTRACE("Allocated {} {} memory!", memoryAllocateInfo.allocationSize, deps.logInfo);
  return UTRUE;
}


b32 FMemoryVulkan::bindBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory deviceMemory) {
  UTRACE("Binding buffer memory...");
  VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(device, buffer, deviceMemory, memoryOffset);
  return UTRUE;
}


b32 FMemoryVulkan::bindImage(VkDevice device, VkImage image, VkDeviceMemory deviceMemory) {
  UTRACE("Binding image memory...");
  VkDeviceSize memoryOffset{ 0 };
  vkBindImageMemory(device, image, deviceMemory, memoryOffset);
  return UTRUE;
}


b32 FMemoryVulkan::copy(const FMemoryCopyDependenciesVulkan& deps) {
  if (deps.copyType == EMemoryCopyType::USE_HOST_MEMCPY) {
    UTRACE("Copying data using host memcpy...");
    b32 copied{ copyDataFromHostToBuffer(deps.device, deps.deviceMemory, deps.size, deps.pData,
                                         deps.logInfo) };
    if (not copied) {
      UERROR("Could not copy using host memcpy!");
      return UFALSE;
    }

    UDEBUG("Copied data using host memcpy!");
    return UTRUE;
  }

  if (deps.copyType == EMemoryCopyType::USE_COMMAND_BUFFER) {
    UTRACE("Copying data using staging buffer...");
    auto stagingDeps{
      reinterpret_cast<const FMemoryCopyBuffersUsingStagingBufferDependenciesVulkan*>(deps.pNext)
    };
    b32 copied{ copyDataFromDeviceToBuffer(deps.device, deps.size, stagingDeps) };
    if (not copied) {
      UERROR("Could not copy using staging buffer!");
      return UFALSE;
    }

    UDEBUG("Copied data using staging buffer!");
    return UTRUE;
  }

  UERROR("Could not find proper way of copy data!");
  return UFALSE;
}


b32 FMemoryVulkan::free(const FMemoryFreeDependenciesVulkan& deps) {
  UTRACE("Freeing memory {}...", deps.logInfo);

  if (*(deps.pDeviceMemory) != VK_NULL_HANDLE) {
    vkFreeMemory(deps.device, *(deps.pDeviceMemory), nullptr);
    *(deps.pDeviceMemory) = VK_NULL_HANDLE;
  }
  else {
    UWARN("As device memory {} is not created, it is not destroyed!", deps.logInfo);
  }

  return UTRUE;
}


b32 FMemoryVulkan::findMemoryIndex(VkPhysicalDevice physicalDevice, u32 typeFilter,
                                   VkMemoryPropertyFlags flags, u32* pOutIndex) {
  VkPhysicalDeviceMemoryProperties memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

  for (u32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
    // Check each memory type to see if its bit is set to 1.
    if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags) {
      UTRACE("Found memory index {}!", i);
      *pOutIndex = i;
      return UTRUE;
    }
  }

  UWARN("Unable to find suitable memory type index!");
  *pOutIndex = UUNUSED;
  return UFALSE;
}


b32 copyDataFromHostToBuffer(VkDevice device, VkDeviceMemory deviceMemory,
                             VkDeviceSize size, void* pData, const char* logInfo) {
  UTRACE("Copying {} of data to {} buffer...", size, logInfo);

  VkDeviceSize offset{ 0 };
  VkMemoryMapFlags flags{ 0 };
  void* pBufferData{ nullptr };
  vkMapMemory(device, deviceMemory, offset, size, flags, &pBufferData);
  memcpy(pBufferData, pData, size);
  vkUnmapMemory(device, deviceMemory);

  UTRACE("Copied {} of data to {} buffer!", size, logInfo);
  return UTRUE;
}


b32 copyDataFromDeviceToBuffer(
    VkDevice device, VkDeviceSize size,
    const FMemoryCopyBuffersUsingStagingBufferDependenciesVulkan* pDeps) {
  UTRACE("Copying {} data from {} buffer to {} buffer...", size, pDeps->srcLogInfo,
         pDeps->dstLogInfo);

  VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandPool = pDeps->transferCommandPool;
  allocateInfo.commandBufferCount = 1;

  VkCommandBuffer transferCommandBuffer{ VK_NULL_HANDLE };
  vkf::AssertResultVulkan( vkAllocateCommandBuffers(device, &allocateInfo, &transferCommandBuffer) );

  VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  beginInfo.pNext = nullptr;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  vkf::AssertResultVulkan( vkBeginCommandBuffer(transferCommandBuffer, &beginInfo) );

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0; // Optional
  copyRegion.dstOffset = 0; // Optional
  copyRegion.size = size;

  vkCmdCopyBuffer(transferCommandBuffer, pDeps->srcBuffer, pDeps->dstBuffer, 1, &copyRegion);

  vkf::AssertResultVulkan( vkEndCommandBuffer(transferCommandBuffer) );

  VkSubmitInfo copySubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  copySubmitInfo.pNext = nullptr;
  copySubmitInfo.waitSemaphoreCount = 0;
  copySubmitInfo.pWaitSemaphores = nullptr;
  copySubmitInfo.pWaitDstStageMask = nullptr;
  copySubmitInfo.commandBufferCount = 1;
  copySubmitInfo.pCommandBuffers = &transferCommandBuffer;
  copySubmitInfo.signalSemaphoreCount = 0;
  copySubmitInfo.pSignalSemaphores = nullptr;

  vkf::AssertResultVulkan( vkQueueSubmit(pDeps->transferQueue, 1, &copySubmitInfo, VK_NULL_HANDLE) );
  vkQueueWaitIdle(pDeps->transferQueue);

  vkFreeCommandBuffers(device, pDeps->transferCommandPool, 1, &transferCommandBuffer);

  UTRACE("Copied {} data from {} buffer to {} buffer!", size, pDeps->srcLogInfo, pDeps->dstLogInfo);
  return UTRUE;
}


}
