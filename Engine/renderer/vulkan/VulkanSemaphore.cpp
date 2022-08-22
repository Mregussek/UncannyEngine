
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


static b32 closeSemaphoresVector(VkDevice device, std::vector<VkSemaphore>* pSemaphoreVector,
                           const char* logInfo);


b32 FRenderContextVulkan::createGraphicsSemaphores() {
  UTRACE("Creating graphics semaphores...");

  u32 imageCount{ mSwapchainDependencies.usedImageCount };

  mVkSemaphoreImageAvailableVector.resize(imageCount);
  mVkSemaphoreRenderingFinishedVector.resize(imageCount);
  mVkSemaphoreCopyImageFinishedVector.resize(imageCount);

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

    VkResult createdCopyImageSemaphore{
        vkCreateSemaphore(mVkDevice, &createInfo, nullptr, &mVkSemaphoreCopyImageFinishedVector[i]) };
    if (createdCopyImageSemaphore != VK_SUCCESS) {
      UERROR("Could not create copy image semaphore {}!", i);
      return UFALSE;
    }
  }

  UDEBUG("Created graphics semaphores!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsSemaphores() {
  UTRACE("Closing graphics semaphores...");

  closeSemaphoresVector(mVkDevice, &mVkSemaphoreImageAvailableVector, "image available");
  closeSemaphoresVector(mVkDevice, &mVkSemaphoreRenderingFinishedVector, "rendering finished");
  closeSemaphoresVector(mVkDevice, &mVkSemaphoreCopyImageFinishedVector, "copy image finished");

  UDEBUG("Closed graphics semaphores!");
  return UTRUE;
}


b32 closeSemaphoresVector(VkDevice device, std::vector<VkSemaphore>* pSemaphoreVector,
                          const char* logInfo) {
  if (pSemaphoreVector->empty()) {
    UWARN("{} semaphores vector is empty! Nothing will be destroyed!", logInfo);
    return UTRUE;
  }

  for (u32 i = 0; i < pSemaphoreVector->size(); i++) {
    if (pSemaphoreVector->at(i) != VK_NULL_HANDLE) {
      UTRACE("Destroying {} semaphore {} ...", logInfo, i);
      vkDestroySemaphore(device, pSemaphoreVector->at(i), nullptr);
    }
    else {
      UWARN("Not destroying {} semaphore {} as it wasn't been created!", logInfo, i);
    }
  }

  pSemaphoreVector->clear();
  return UTRUE;
}


}
