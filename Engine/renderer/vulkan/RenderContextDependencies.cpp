
#include "RenderContextVulkan.h"
#include "RenderContextDependencies.h"
#include <utilities/Logger.h>


namespace uncanny
{


FQueueFamilyDependencies getQueueFamilyDependencies(
    EQueueFamilyMainUsage mainUsage,
    const std::vector<FQueueFamilyDependencies>& queueFamilyDependenciesVector) {
  auto it = std::find_if(queueFamilyDependenciesVector.begin(), queueFamilyDependenciesVector.end(),
                         [mainUsage](const FQueueFamilyDependencies& dependencies) -> b32 {
                           return dependencies.mainUsage == mainUsage;
                         });
  if (it != queueFamilyDependenciesVector.end()) {
    UTRACE("Found queue family dependencies with main usage {}", (i32)mainUsage);
    return *it;
  }

  UTRACE("Could not find queue family dependencies with main usage {}", (i32)mainUsage);
  return {};
}


static b32 validateInstanceDependencies(const FInstanceDependencies& instanceDeps);


static b32 validatePhysicalDeviceDependencies(const FPhysicalDeviceDependencies& physDevDeps);


static b32 validateSwapchainDependencies(const FSwapchainDependencies& swapchainDeps);


static b32 validateImageDependencies(const FImagesDependencies& imageDeps);


static b32 validateSpecificImageDependencies(
    const FSpecificImageDependencies& specificImageDeps, const char* logInfo);


b32 FRenderContextVulkan::validateDependencies() const {
  UTRACE("Validating dependencies for vulkan renderer...");

  if (not validateInstanceDependencies(mInstanceDependencies)) {
    UERROR("Wrong instance dependencies!");
    return UFALSE;
  }
  if (not validatePhysicalDeviceDependencies(mPhysicalDeviceDependencies)) {
    UERROR("Wrong physical device dependencies!");
    return UFALSE;
  }
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


b32 validateInstanceDependencies(const FInstanceDependencies& instanceDeps) {
  UTRACE("Validating instance deps...");

  // make sure proper vulkan api version is given
  if (instanceDeps.vulkanApiVersion == 0) {
    UERROR("Wrong Vulkan API version given in dependencies!");
    return UFALSE;
  }

  UTRACE("Validated instance deps!");
  return UTRUE;
}


b32 validatePhysicalDeviceDependencies(const FPhysicalDeviceDependencies& physDevDeps) {
  UTRACE("Validating physical device deps...");

  // make sure there is proper queue family index count
  if (physDevDeps.queueFamilyIndexesCount < 1) {
    UERROR("Queue family indexes count is less than 1, which means no operations on GPU!");
    return UFALSE;
  }

  // make sure device type is possible to select
  if (
      physDevDeps.deviceType == EPhysicalDeviceType::NONE or
      physDevDeps.deviceTypeFallback == EPhysicalDeviceType::NONE) {
    UERROR("Defined device types are NONE, which means proper GPU cannot be selected!");
    return UFALSE;
  }

  // make sure there is enough queue dependencies for every queue family
  if (physDevDeps.queueFamilyIndexesCount > physDevDeps.queueFamilyDependencies.size()) {
    UERROR("There is more queue families than dependencies for queue families, not enough info!");
    return UFALSE;
  }

  // make sure that every queue of expected queue family is correctly defined
  for (u32 i = 0; i < physDevDeps.queueFamilyIndexesCount; i++) {
    const auto& queueDeps{ physDevDeps.queueFamilyDependencies[i] }; // wrapper

    // make sure there is proper queue for queue family
    if (queueDeps.queuesCountNeeded < 1) {
      UERROR("Needed queues for family are less than 1, which means no operations on GPU!");
      return UFALSE;
    }

    // make sure that is sth supported for queue
    if (
        (not queueDeps.graphics) && (not queueDeps.compute) &&
        (not queueDeps.transfer) && (not queueDeps.sparseBinding)) {
      UERROR("There is no support info for queue provided, not enough info!");
      return UFALSE;
    }

    // make sure there is enough info about every queue needed for queue family
    if (queueDeps.queuesCountNeeded > queueDeps.queuesPriorities.size()) {
      UERROR("There is more queues need than provided info about them!");
      return UFALSE;
    }

    // make sure there is proper queue type
    if (queueDeps.mainUsage == EQueueFamilyMainUsage::NONE) {
      UERROR("Queue type for queue family is NONE, wrong info provided!");
      return UFALSE;
    }

    // make sure every queue has correct info provided
    for (u32 j = 0; j < queueDeps.queuesCountNeeded; j++) {
      // make sure priority is between range
      if (not (0.f <= queueDeps.queuesPriorities[j] && queueDeps.queuesPriorities[j] <= 1.f)) {
        UERROR("Queue priority is not between range <0.f, 1.f>, wrong info provided!");
        return UFALSE;
      }
    }
  }

  UTRACE("Validated physical device deps!");
  return UTRUE;
}


b32 validateSwapchainDependencies(const FSwapchainDependencies& swapchainDeps) {
  UTRACE("Validating swapchain dependencies...");

  if (swapchainDeps.usedImageCount < 2) {
    UERROR("Minimal image count for swapchain is 2, back and front buffers! Wrong dependencies!");
    return UFALSE;
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
  for (VkFormat format : specificImageDeps.formatCandidatesVector) {
    if (format == VK_FORMAT_UNDEFINED) {
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

  UDEBUG("Validated specific {} image deps!", logInfo);
  return UTRUE;
}


}
