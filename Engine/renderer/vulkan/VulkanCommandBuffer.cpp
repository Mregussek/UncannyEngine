
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createCommandBuffer() {
  UTRACE("Creating command buffers...");

  VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = mVkGraphicsCommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = 1;

  U_VK_ASSERT( vkAllocateCommandBuffers(mVkDevice, &allocateInfo, &mVkGraphicsCommandBuffer) );

  UDEBUG("Created command buffers!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeCommandBuffer() {
  UTRACE("Closing command buffers...");

  if (mVkGraphicsCommandBuffer != VK_NULL_HANDLE) {
    UTRACE("Freeing graphics command buffer...");
    vkFreeCommandBuffers(mVkDevice, mVkGraphicsCommandPool, 1, &mVkGraphicsCommandBuffer);
  }
  else {
    UWARN("Not freeing graphics command buffer as it wasn't been created!");
  }

  UDEBUG("Closed command buffers!");
  return UTRUE;
}


}
