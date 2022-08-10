
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createCommandPool() {
  UTRACE("Creating command pools...");

  VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
  createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  createInfo.queueFamilyIndex = mGraphicsQueueFamilyIndex;

  U_VK_ASSERT( vkCreateCommandPool(mVkDevice, &createInfo, nullptr, &mVkGraphicsCommandPool) );

  UDEBUG("Created command pools!");
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

  UDEBUG("Closed command pools!");
  return UTRUE;
}


}
