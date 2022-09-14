
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createGraphicsFences() {
  UTRACE("Creating graphics fences...");

  u32 imageCount{ mSwapchainDependencies.usedImageCount };
  mVkFencesInFlightFrames.resize(imageCount);

  VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (u32 i = 0; i < imageCount; i++) {
    U_VK_ASSERT( vkCreateFence(mContextPtr->Device(), &createInfo, nullptr,
                               &mVkFencesInFlightFrames[i]) );
  }

  UDEBUG("Created graphics fences!");
  return UTRUE;
}


b32 FRendererVulkan::closeGraphicsFences() {
  UTRACE("Closing graphics fences...");

  if (mVkFencesInFlightFrames.empty()) {
    UWARN("Graphics fences vector is empty, so nothing will be destroyed!");
    return UTRUE;
  }

  for (u32 i = 0; i < mVkFencesInFlightFrames.size(); i++) {
    UTRACE("Destroying graphics fence {}...", i);
    if (mVkFencesInFlightFrames[i] != VK_NULL_HANDLE) {
      vkDestroyFence(mContextPtr->Device(), mVkFencesInFlightFrames[i], nullptr);
      mVkFencesInFlightFrames[i] = VK_NULL_HANDLE;
      continue;
    }
    UWARN("As graphics fence {} is not created, so it is not destroyed!", i);
  }
  mVkFencesInFlightFrames.clear();

  UDEBUG("Closed graphics fences!");
  return UTRUE;
}


}