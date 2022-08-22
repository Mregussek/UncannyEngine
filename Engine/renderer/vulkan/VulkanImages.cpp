
#include "VulkanImages.h"
#include <utilities/Logger.h>
#include "RenderContextStructures.h"


namespace uncanny
{


b32 closeImageVulkan(FImageVulkan* pImage, VkDevice device, const char* logInfo) {
  if (pImage->format != VK_FORMAT_UNDEFINED) {
    UTRACE("Clearing {} format variable...", logInfo);
    pImage->format = VK_FORMAT_UNDEFINED;
  }

  if (pImage->handleView != VK_NULL_HANDLE) {
    UTRACE("Destroying {} image view...", logInfo);
    vkDestroyImageView(device, pImage->handleView, nullptr);
  }
  else {
    UWARN("As {} image view is not created, it is not destroyed!", logInfo);
  }

  // It is nice to firstly destroy image, then free its memory, as
  // if image will be used it will be referencing freed memory
  if (pImage->handle != VK_NULL_HANDLE) {
    UTRACE("Destroying {} image...", logInfo);
    vkDestroyImage(device, pImage->handle, nullptr);
  }
  else {
    UWARN("As {} image is not created, it is not destroyed!", logInfo);
  }

  if (pImage->deviceMemory != VK_NULL_HANDLE) {
    UTRACE("Freeing {} image memory...", logInfo);
    vkFreeMemory(device, pImage->deviceMemory, nullptr);
  }
  else {
    UWARN("As {} image memory is not allocated, it won't be freed!", logInfo);
  }

  return UTRUE;
}


}
