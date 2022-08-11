
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createCommandBuffer() {
  UTRACE("Creating command buffers...");

  mVkGraphicsCommandBufferVector.resize(mSwapchainDependencies.usedImageCount);

  VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = mVkGraphicsCommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = mSwapchainDependencies.usedImageCount;

  U_VK_ASSERT( vkAllocateCommandBuffers(mVkDevice, &allocateInfo,
                                        mVkGraphicsCommandBufferVector.data()) );

  UDEBUG("Created command buffers!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeCommandBuffer() {
  UTRACE("Closing command buffers...");

  vkDeviceWaitIdle(mVkDevice);

  for (u32 i = 0; i < mVkGraphicsCommandBufferVector.size(); i++) {
    if (mVkGraphicsCommandBufferVector[i] != VK_NULL_HANDLE) {
      UTRACE("Freeing graphics command buffer {} ...", i);
      vkFreeCommandBuffers(mVkDevice, mVkGraphicsCommandPool, 1, &mVkGraphicsCommandBufferVector[i]);
    }
    else {
      UWARN("Not freeing graphics command buffer {} as it wasn't been created!", i);
    }
  }
  mVkGraphicsCommandBufferVector.clear();

  UDEBUG("Closed command buffers!");
  return UTRUE;
}


}
