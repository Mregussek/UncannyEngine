
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


b32 FRenderContextVulkan::validateDependencies() const {
  UTRACE("Validating dependencies for vulkan renderer...");
  const auto& physDevDeps{ mPhysicalDeviceDependencies }; // wrapper

  // make sure proper vulkan api version is given
  if (mInstanceDependencies.vulkanApiVersion == 0) {
    UERROR("Wrong Vulkan API version given in dependencies!");
    return UFALSE;
  }

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

  // make sure that depth dependencies are correct
  if (physDevDeps.depthFormatDependencies.empty()) {
    UERROR("No depth dependencies info!");
    return UFALSE;
  }

  // make sure that there is not undefined depth format
  for (VkFormat depthFormat : physDevDeps.depthFormatDependencies) {
    if (depthFormat == VK_FORMAT_UNDEFINED) {
      UERROR("One of depth formats is undefined, check it!");
      return UFALSE;
    }
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

  if (mSwapchainDependencies.usedImageCount < 2) {
    UERROR("Minimal image count for swapchain is 2, back and front buffers! Wrong dependencies!");
    return UFALSE;
  }

  if (mSwapchainDependencies.imageUsageVector.empty()) {
    UERROR("There is no info about image usage in vector!");
    return UFALSE;
  }

  for (VkImageUsageFlagBits imageUsage: mSwapchainDependencies.imageUsageVector) {
    if (imageUsage == 0) {
      UERROR("Image usage is not defined! 0 value in vector, wrong info given!");
      return UFALSE;
    }
  }

  UDEBUG("Properly defined dependencies for vulkan renderer!");
  return UTRUE;
}


}
