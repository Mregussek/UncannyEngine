
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsSemaphores() {
  UTRACE("Creating graphics semaphores...");

  VkSemaphoreCreateInfo createInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;

  VkResult createdImageAvailableSemaphore{ vkCreateSemaphore(mVkDevice, &createInfo, nullptr,
                                                             &mVkSemaphoreImageAvailable) };
  if (createdImageAvailableSemaphore != VK_SUCCESS) {
    UERROR("Could not create image available semaphore!");
    return UFALSE;
  }

  VkResult createdRenderFinishSemaphore{ vkCreateSemaphore(mVkDevice, &createInfo, nullptr,
                                                           &mVkSemaphoreRenderingFinished) };
  if (createdRenderFinishSemaphore != VK_SUCCESS) {
    UERROR("Could not create render finished semaphore!");
    return UFALSE;
  }

  UDEBUG("Created graphics semaphores!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsSemaphores() {
  UTRACE("Closing graphics semaphores...");

  if (mVkSemaphoreImageAvailable != VK_NULL_HANDLE) {
    UTRACE("Destroying image available semaphore...");
    vkDestroySemaphore(mVkDevice, mVkSemaphoreImageAvailable, nullptr);
  }
  else {
    UWARN("Not destroying image available semaphore as it wasn't been created!");
  }

  if (mVkSemaphoreRenderingFinished != VK_NULL_HANDLE) {
    UTRACE("Destroying render finished semaphore...");
    vkDestroySemaphore(mVkDevice, mVkSemaphoreRenderingFinished, nullptr);
  }
  else {
    UWARN("Not destroying render finished semaphore as it wasn't been created!");
  }

  UDEBUG("Closed graphics semaphores!");
  return UTRUE;
}


}
