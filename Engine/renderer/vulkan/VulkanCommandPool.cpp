
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createCommandPool() {
  UTRACE("Creating command pools...");

  // even when VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT is set use vkResetCommandPool
  VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
  createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  createInfo.queueFamilyIndex = mGraphicsQueueFamilyIndex;

  U_VK_ASSERT( vkCreateCommandPool(mVkDevice, &createInfo, nullptr, &mVkGraphicsCommandPool) );

  createInfo.queueFamilyIndex = mTransferQueueFamilyIndex;

  U_VK_ASSERT( vkCreateCommandPool(mVkDevice, &createInfo, nullptr, &mVkTransferCommandPool) );

  UDEBUG("Created command pools!");
  return UTRUE;
}


b32 FRenderContextVulkan::resetCommandPool(const VkCommandPool& commandPool) {
  UTRACE("Resetting command pools...");

  VkCommandPoolResetFlags flags = 0;
  VkResult result{ vkResetCommandPool(mVkDevice, commandPool, flags) };
  if (result != VK_SUCCESS) {
    UERROR("Could not reset command pools!");
    return UFALSE;
  }

  UDEBUG("Reset command pools!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeCommandPool() {
  UTRACE("Closing command pools...");

  if (mVkGraphicsCommandPool != VK_NULL_HANDLE) {
    UTRACE("Destroying graphics command pool...");
    vkDestroyCommandPool(mVkDevice, mVkGraphicsCommandPool, nullptr);
  }
  else {
    UWARN("As graphics command pool was not created, it won't be closed!");
  }

  if (mVkTransferCommandPool != VK_NULL_HANDLE) {
    UTRACE("Destroying transfer command pool...");
    vkDestroyCommandPool(mVkDevice, mVkTransferCommandPool, nullptr);
  }
  else {
    UWARN("As transfer command pool was not created, it won't be closed!");
  }

  UDEBUG("Closed command pools!");
  return UTRUE;
}


}
