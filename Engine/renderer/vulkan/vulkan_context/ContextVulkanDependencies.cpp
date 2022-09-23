
#include "ContextVulkan.h"
#include "ContextVulkanDependencies.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 getQueueFamilyDependencies(
    EQueueFamilyMainUsage mainUsage,
    const std::vector<FQueueFamilyDependencies>& queueFamilyDependenciesVector,
    FQueueFamilyDependencies* pOutput) {
  for (const FQueueFamilyDependencies& deps : queueFamilyDependenciesVector) {
    if (deps.mainUsage == mainUsage) {
      UTRACE("Found queue family dependencies with main usage {}", (i32)mainUsage);
      *pOutput = deps;
      return UTRUE;
    }
  }

  UTRACE("Could not find queue family dependencies with main usage {}", (i32)mainUsage);
  return UFALSE;
}


static b32 validateInstanceDependencies(const FInstanceDependencies& instanceDeps);


static b32 validatePhysicalDeviceDependencies(const FPhysicalDeviceDependencies& physDevDeps);


static b32 validateWindowSurfaceDependencies(const FWindowSurfaceDependencies& surfaceDeps);


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
  if (not validateWindowSurfaceDependencies(mWindowSurfaceDependencies)) {
    UERROR("Wrong window surface dependencies!");
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


b32 validateWindowSurfaceDependencies(const FWindowSurfaceDependencies& surfaceDeps) {
  UTRACE("Validating window surface deps...");

  if (surfaceDeps.presentModeCandidates.empty()) {
    UERROR("No window surface present mode candidates info!");
    return UFALSE;
  }
  // There is no need to validate present mode, as default one is 0-valued

  UTRACE("Validated window surface deps!");
  return UTRUE;
}


}
