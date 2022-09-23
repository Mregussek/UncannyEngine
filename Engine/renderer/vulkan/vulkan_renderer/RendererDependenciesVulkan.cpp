
#include "RendererVulkan.h"
#include "RendererDependenciesVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


static b32 validateSwapchainDependencies(const FSwapchainDependencies& swapchainDeps);


static b32 validateImageDependencies(const FImagesDependencies& imageDeps);


static b32 validateSpecificImageDependencies(
    const FSpecificImageDependencies& specificImageDeps, const char* logInfo);


b32 FRendererVulkan::validateDependencies() const {
  UTRACE("Validating dependencies for vulkan renderer...");

  if (not validateSwapchainDependencies(mSwapchainDependencies)) {
    UERROR("Wrong swapchain dependencies!");
    return UFALSE;
  }
  if (not validateImageDependencies(mImageDependencies)) {
    UERROR("Wrong image dependencies!");
    return UFALSE;
  }

  UDEBUG("Properly defined dependencies for vulkan renderer!");
  return UTRUE;
}


b32 validateSwapchainDependencies(const FSwapchainDependencies& swapchainDeps) {
  UTRACE("Validating swapchain dependencies...");

  if (swapchainDeps.usedImageCount < 2) {
    UERROR("Minimal image count for swapchain is 2, back and front buffers! Wrong dependencies!");
    return UFALSE;
  }

  if (swapchainDeps.formatCandidatesVector.empty()) {
    UERROR("There is no info about format in vector!");
    return UFALSE;
  }
  for (VkSurfaceFormatKHR format : swapchainDeps.formatCandidatesVector) {
    if (format.format == VK_FORMAT_UNDEFINED) {
      UERROR("Swapchain image format is not defined! Undefined value in vector, wrong info given!");
      return UFALSE;
    }
  }

  if (swapchainDeps.imageUsageVector.empty()) {
    UERROR("There is no info about image usage in vector!");
    return UFALSE;
  }

  for (VkImageUsageFlags imageUsage: swapchainDeps.imageUsageVector) {
    if (imageUsage == 0) {
      UERROR("Image usage is not defined! 0 value in vector, wrong info given!");
      return UFALSE;
    }
  }

  if (swapchainDeps.imageFormatFeatureVector.empty()) {
    UERROR("No swapchain image format features info!");
    return UFALSE;
  }
  // make sure that there is not undefined format feature
  for (VkFormatFeatureFlags formatFeature : swapchainDeps.imageFormatFeatureVector) {
    if (formatFeature == 0) {
      UERROR("One of swapchain image format features is undefined, check it!");
      return UFALSE;
    }
  }

  UTRACE("Validated swapchain dependencies!");
  return UTRUE;
}


b32 validateImageDependencies(const FImagesDependencies& imageDeps) {
  UTRACE("Validating image dependencies...");

  if (not validateSpecificImageDependencies(imageDeps.renderTarget, "render target")) {
    UERROR("Wrong render target image dependencies!");
    return UFALSE;
  }
  if (not validateSpecificImageDependencies(imageDeps.depth, "depth")) {
    UERROR("Wrong depth image dependencies!");
    return UFALSE;
  }

  UTRACE("Correct image dependencies!");
  return UTRUE;
}


b32 validateSpecificImageDependencies(
    const FSpecificImageDependencies& specificImageDeps, const char* logInfo) {
  UTRACE("Validating specific {} image deps...", logInfo);

  if (specificImageDeps.formatCandidatesVector.empty()) {
    UERROR("No {} format candidates info!", logInfo);
    return UFALSE;
  }
  if (specificImageDeps.formatsFeatureVector.empty()) {
    UERROR("No {} format features info!", logInfo);
    return UFALSE;
  }
  if (specificImageDeps.usageVector.empty()) {
    UERROR("No {} image usage info!", logInfo);
    return UFALSE;
  }

  // make sure that there is not undefined format
  for (VkSurfaceFormatKHR format : specificImageDeps.formatCandidatesVector) {
    if (format.format == VK_FORMAT_UNDEFINED) {
      UERROR("One of {} image formats is undefined, check it!", logInfo);
      return UFALSE;
    }
  }

  // make sure that there is not undefined format feature
  for (VkFormatFeatureFlags formatFeature : specificImageDeps.formatsFeatureVector) {
    if (formatFeature == 0) {
      UERROR("One of {} format features is undefined, check it!", logInfo);
      return UFALSE;
    }
  }

  // make sure that there is not undefined image usage feature
  for (VkImageUsageFlags imageUsage : specificImageDeps.usageVector) {
    if (imageUsage == 0) {
      UERROR("One of {} image usage is undefined, check it!", logInfo);
      return UFALSE;
    }
  }

  UTRACE("Validated specific {} image deps!", logInfo);
  return UTRUE;
}


}
