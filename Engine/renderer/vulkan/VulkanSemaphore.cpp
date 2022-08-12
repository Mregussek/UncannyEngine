
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsSemaphores() {
  UTRACE("Creating graphics semaphores...");

  u32 imageCount{ mSwapchainDependencies.usedImageCount };

  mVkSemaphoreImageAvailableVector.resize(imageCount);
  mVkSemaphoreRenderingFinishedVector.resize(imageCount);

  VkSemaphoreCreateInfo createInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;

  for (u32 i = 0; i < imageCount; i++) {
    VkResult createdImageAvailableSemaphore{
      vkCreateSemaphore(mVkDevice, &createInfo, nullptr, &mVkSemaphoreImageAvailableVector[i]) };
    if (createdImageAvailableSemaphore != VK_SUCCESS) {
      UERROR("Could not create image available semaphore {}!", i);
      return UFALSE;
    }

    VkResult createdRenderFinishSemaphore{
      vkCreateSemaphore(mVkDevice, &createInfo, nullptr, &mVkSemaphoreRenderingFinishedVector[i]) };
    if (createdRenderFinishSemaphore != VK_SUCCESS) {
      UERROR("Could not create render finished semaphore {}!", i);
      return UFALSE;
    }
  }

  UDEBUG("Created graphics semaphores!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsSemaphores() {
  UTRACE("Closing graphics semaphores...");

  if (mVkSemaphoreImageAvailableVector.empty()) {
    UWARN("Image available semaphores vector is empty! Nothing will be destroyed!");
  }

  if (mVkSemaphoreRenderingFinishedVector.empty()) {
    UWARN("Render finish semaphores vector is empty! Nothing will be destroyed!");
  }

  if (mVkSemaphoreImageAvailableVector.size() != mVkSemaphoreRenderingFinishedVector.size()) {
    UERROR("Semaphores image available and render finish has different sizes! ImAv {} ReFi {}",
           mVkSemaphoreImageAvailableVector.size(), mVkSemaphoreRenderingFinishedVector.size());
  }

  for (u32 i = 0; i < mVkSemaphoreImageAvailableVector.size(); i++) {
    if (mVkSemaphoreImageAvailableVector[i] != VK_NULL_HANDLE) {
      UTRACE("Destroying image available semaphore {} ...", i);
      vkDestroySemaphore(mVkDevice, mVkSemaphoreImageAvailableVector[i], nullptr);
    }
    else {
      UWARN("Not destroying image available semaphore {} as it wasn't been created!", i);
    }
  }

  for (u32 i = 0; i < mVkSemaphoreRenderingFinishedVector.size(); i++) {
    if (mVkSemaphoreRenderingFinishedVector[i] != VK_NULL_HANDLE) {
      UTRACE("Destroying render finished semaphore {} ...", i);
      vkDestroySemaphore(mVkDevice, mVkSemaphoreRenderingFinishedVector[i], nullptr);
    }
    else {
      UWARN("Not destroying render finished semaphore {} as it wasn't been created!", i);
    }
  }

  UDEBUG("Closed graphics semaphores!");
  return UTRUE;
}


}
