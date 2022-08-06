
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FRenderContextVulkan::defineDependencies() {
  UTRACE("Defining dependencies...");

  FQueueFamilyDependencies graphicsQueueFamilyDependencies{};
  graphicsQueueFamilyDependencies.queuesCountNeeded = 2;
  graphicsQueueFamilyDependencies.queuesPriorities = { 1.f, 1.f };
  graphicsQueueFamilyDependencies.queuesTypes = { EQueueType::RENDERING, EQueueType::PRESENTING };
  graphicsQueueFamilyDependencies.graphics = UTRUE;

  mPhysicalDeviceDependencies.deviceType = EPhysicalDeviceType::DISCRETE;
  mPhysicalDeviceDependencies.deviceTypeFallback = EPhysicalDeviceType::INTEGRATED;
  mPhysicalDeviceDependencies.queueFamilyIndexesCount = 1;
  mPhysicalDeviceDependencies.queueFamilyDependencies = { graphicsQueueFamilyDependencies };
}


b32 FRenderContextVulkan::validateDependencies() const {
  UTRACE("Validating dependencies for vulkan renderer...");

  // make sure there is proper queue family index count
  if (mPhysicalDeviceDependencies.queueFamilyIndexesCount < 1) {
    UERROR("Queue family indexes count is less than 1, which means no operations on GPU!");
    return UFALSE;
  }

  // make sure device type is possible to select
  if (
      mPhysicalDeviceDependencies.deviceType == EPhysicalDeviceType::NONE or
      mPhysicalDeviceDependencies.deviceTypeFallback == EPhysicalDeviceType::NONE) {
    UERROR("Defined device types are NONE, which means proper GPU cannot be selected!");
    return UFALSE;
  }

  // make sure there is enough queue dependencies for every queue family
  if (
      mPhysicalDeviceDependencies.queueFamilyIndexesCount >
      mPhysicalDeviceDependencies.queueFamilyDependencies.size()) {
    UERROR("There is more queue families than dependencies for queue families, not enough info!");
    return UFALSE;
  }

  // make sure that every queue of expected queue family is correctly defined
  for (const FQueueFamilyDependencies& queueDeps : mPhysicalDeviceDependencies.queueFamilyDependencies) {
    // make sure there is proper queue for queue family
    if (queueDeps.queuesCountNeeded < 1) {
      UERROR("Needed queues for family are less than 1, which means no operations on GPU!");
      return UFALSE;
    }

    if (
        (not queueDeps.graphics) && (not queueDeps.compute) &&
        (not queueDeps.transfer) && (not queueDeps.sparseBinding)) {
      UERROR("There is no support info for queue provided, not enough info!");
      return UFALSE;
    }

    // make sure there is enough info about every queue needed for queue family
    if (
        queueDeps.queuesCountNeeded > queueDeps.queuesPriorities.size() or
        queueDeps.queuesCountNeeded > queueDeps.queuesTypes.size()) {
      UERROR("There is more queues need than provided info about them!");
      return UFALSE;
    }

    // make sure every queue has correct info provided
    for (u32 j = 0; j < queueDeps.queuesCountNeeded; j++) {
      // make sure there is proper queue type
      if (queueDeps.queuesTypes[j] == EQueueType::NONE) {
        UERROR("Queue type for queue family is NONE, wrong info provided!");
        return UFALSE;
      }

      // make sure priority is between range
      if (not (0.f <= queueDeps.queuesPriorities[j] && queueDeps.queuesPriorities[j] <= 1.f)) {
        UERROR("Queue priority is not between range <0.f, 1.f>, wrong info provided!");
        return UFALSE;
      }
    }
  }

  UDEBUG("Properly defined dependencies for vulkan renderer!");
  return UTRUE;
}


b32 FRenderContextVulkan::init(FRenderContextSpecification renderContextSpecs) {
  UTRACE("Initializing Vulkan Render Context...");

  // assign specs
  mSpecs = renderContextSpecs;

  // define all dependencies for vulkan renderer before setup
  defineDependencies();
  b32 properDependenciesDefined{ validateDependencies() };
  if (not properDependenciesDefined) {
    UFATAL("Wrong dependencies defined for vulkan renderer!");
    return UFALSE;
  }

  // Firstly creating instance as it is mandatory for proper Vulkan work...
  b32 properlyCreatedInstance{ createInstance() };
  if (not properlyCreatedInstance) {
    UFATAL("Could not create Vulkan Instance!");
    return UFALSE;
  }

  // Additionally if flag is enabled create vulkan debugger
  if constexpr (U_VK_DEBUG) {
    b32 properlyCreatedDebugUtilsMsg{ createDebugUtilsMessenger() };
    if (not properlyCreatedDebugUtilsMsg) {
      UERROR("Could not create Debug Utils Messenger!");
      return UFALSE;
    }
  }

  // Secondly creating physical device (needed for logical device)...
  b32 properlyCreatedPhysicalDevice{ createPhysicalDevice() };
  if (not properlyCreatedPhysicalDevice) {
    UFATAL("Could not pick Vulkan Physical Device!");
    return UFALSE;
  }

  // We can create window surface (dependent only on VkInstance) and check presentation
  // support with physical device that is why third step is window surface creation...
  b32 properlyCreatedWindowSurface{ createWindowSurface() };
  if (not properlyCreatedWindowSurface) {
    UFATAL("Could not create window surface, rendering is not available!");
    return UFALSE;
  }

  // We create logical device (dependent only on VkPhysicalDevice)
  b32 properlyCreatedLogicalDevice{ createLogicalDevice() };
  if (not properlyCreatedLogicalDevice) {
    UFATAL("Could not create logical device!");
    return UFALSE;
  }

  // Assigning all graphics command queues for rendering operations
  b32 properlyCreatedGraphicsQueues{ createGraphicsQueues() };
  if (not properlyCreatedGraphicsQueues) {
    UFATAL("There is no graphics queues, rendering in not available!");
    return UFALSE;
  }

  // we can create swapchain for acquiring images and presenting them
  b32 properlyCreatedSwapchain{ createSwapchain() };
  if (not properlyCreatedSwapchain) {
    UFATAL("Could not create swapchain, cannot acquire images and present them!");
    return UFALSE;
  }

  UINFO("Initialized Vulkan Render Context!");
  return UTRUE;
}


void FRenderContextVulkan::terminate() {
  UTRACE("Terminating Vulkan Render Context...");

  closeSwapchain();
  closeGraphicsQueues();
  closeLogicalDevice();
  closeWindowSurface();
  closePhysicalDevice();
  if constexpr (U_VK_DEBUG) {
    closeDebugUtilsMessenger();
  }
  closeInstance();

  UINFO("Terminated Vulkan Render Context!");
}


}
