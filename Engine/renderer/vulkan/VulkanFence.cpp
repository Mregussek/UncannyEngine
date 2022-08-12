
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsFences() {
  UTRACE("Creating graphics fences...");

  u32 commandBuffersCount{ (u32)mVkGraphicsCommandBufferVector.size() };
  mVkGraphicsFenceVector.resize(commandBuffersCount);

  VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;

  for (u32 i = 0; i < commandBuffersCount; i++) {
    U_VK_ASSERT( vkCreateFence(mVkDevice, &createInfo, nullptr, &mVkGraphicsFenceVector[i]) );
  }

  UDEBUG("Created graphics fences!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsFences() {
  UTRACE("Closing graphics fences...");

  if (mVkGraphicsFenceVector.empty()) {
    UWARN("Graphics fences vector is empty, so nothing will be destroyed!");
    return UTRUE;
  }

  for (u32 i = 0; i < mVkGraphicsFenceVector.size(); i++) {
    UTRACE("Destroying graphics fence {}...", i);
    if (mVkGraphicsFenceVector[i] != VK_NULL_HANDLE) {
      vkDestroyFence(mVkDevice, mVkGraphicsFenceVector[i], nullptr);
      mVkGraphicsFenceVector[i] = VK_NULL_HANDLE;
      continue;
    }
    UWARN("As graphics fence {} is not created, so it is not destroyed!", i);
  }
  mVkGraphicsFenceVector.clear();

  UDEBUG("Closed graphics fences!");
  return UTRUE;
}


}
