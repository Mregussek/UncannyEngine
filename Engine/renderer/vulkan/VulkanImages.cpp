
#include "VulkanImages.h"
#include <utilities/Logger.h>
#include "RenderContextStructures.h"


namespace uncanny
{


b32 closeImageVulkan(FImageVulkan* pImage, VkDevice device, const char* logInfo) {
  if (pImage->handleView != VK_NULL_HANDLE) {
    UTRACE("Destroying {} image view...", logInfo);
    vkDestroyImageView(device, pImage->handleView, nullptr);
  }
  else {
    UWARN("As {} image view is not created, it is not destroyed!", logInfo);
  }

  // It is nice to firstly destroy image, then free its memory, as
  // if image will be used it will be referencing freed memory
  if (pImage->handle != VK_NULL_HANDLE and pImage->type != EImageType::PRESENTABLE) {
    UTRACE("Destroying {} image...", logInfo);
    vkDestroyImage(device, pImage->handle, nullptr);
  }
  else {
    if (pImage->type != EImageType::PRESENTABLE) {
      UWARN("As {} image is not created, it is not destroyed!", logInfo);
    }
    else {
      UTRACE("As {} image handle is handled by swapchain, it is not freed here!", logInfo);
    }
  }

  if (pImage->deviceMemory != VK_NULL_HANDLE) {
    UTRACE("Freeing {} image memory...", logInfo);
    vkFreeMemory(device, pImage->deviceMemory, nullptr);
  }
  else {
    if (pImage->type != EImageType::PRESENTABLE) {
      UWARN("As {} image memory is not allocated, it won't be freed!", logInfo);
    }
    else {
      UTRACE("As {} image memory is handled by swapchain, it is not freed here!", logInfo);
    }
  }

  if (pImage->format != VK_FORMAT_UNDEFINED) {
    UTRACE("Clearing {} image format variable...", logInfo);
    pImage->format = VK_FORMAT_UNDEFINED;
  }

  if (pImage->type != EImageType::NONE) {
    UTRACE("Clearing {} image type variable...", logInfo);
    pImage->type = EImageType::NONE;
  }

  return UTRUE;
}


static b32 areTilingFeaturesMetForImageFormat(
    VkFormatFeatureFlags deviceFormatFeatureFlags,
    const std::vector<VkFormatFeatureFlags>& formatFeatureDependenciesVector);


b32 areFormatsFeaturesDependenciesMetForImageFormat(
    VkFormat imageFormat, VkImageTiling tiling, VkPhysicalDevice physicalDevice,
    const std::vector<VkFormatFeatureFlags>& formatFeatureVector, const char* logInfo) {
  UTRACE("Validating {} image format features dependencies...", logInfo);

  VkFormatProperties formatProperties{};
  vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

  if (tiling == VK_IMAGE_TILING_OPTIMAL) {
    b32 tilingFeaturesSupported{ areTilingFeaturesMetForImageFormat(
        formatProperties.optimalTilingFeatures, formatFeatureVector) };
    if (not tilingFeaturesSupported) {
      UERROR("Some Optimal Format Feature Flag is not supported by format {} for {} image!",
             imageFormat, logInfo);
      return UFALSE;
    }
  }
  else {
    b32 tilingFeaturesSupported{ areTilingFeaturesMetForImageFormat(
        formatProperties.linearTilingFeatures, formatFeatureVector) };
    if (not tilingFeaturesSupported) {
      UERROR("Some Linear Format Feature Flag is not supported by format {} for {} image!",
             imageFormat, logInfo);
      return UFALSE;
    }
  }

  UDEBUG("Validated {} image format features dependencies, it seems correct!", logInfo);
  return UTRUE;
}


b32 areTilingFeaturesMetForImageFormat(
    VkFormatFeatureFlags deviceFormatFeatureFlags,
    const std::vector<VkFormatFeatureFlags>& formatFeatureDependenciesVector) {
  for (VkFormatFeatureFlags featureFlag : formatFeatureDependenciesVector) {
    if (not (deviceFormatFeatureFlags & featureFlag)) {
      UERROR("Format Feature flag {} is not supported!", featureFlag);
      return UFALSE;
    }
  }

  UTRACE("Found every format feature flag supported!");
  return UTRUE;
}


}
