
#include "ContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FRenderContextVulkan::defineDependencies() {
  UTRACE("Defining context dependencies...");

  // INSTANCE
  mInstanceDependencies.vulkanApiVersion = VK_API_VERSION_1_3;

  // PHYSICAL DEVICE
  // My AMD card support only one 1 queue at graphics queue family, my NVIDIA supports 16!
  FQueueFamilyDependencies graphicsQueueFamilyDependencies{};
  graphicsQueueFamilyDependencies.mainUsage = EQueueFamilyMainUsage::GRAPHICS;
  graphicsQueueFamilyDependencies.queuesCountNeeded = 1;
  graphicsQueueFamilyDependencies.queuesPriorities = { 1.f, 1.f };
  graphicsQueueFamilyDependencies.graphics = UTRUE;

  FQueueFamilyDependencies transferQueueFamilyDependencies{};
  transferQueueFamilyDependencies.mainUsage = EQueueFamilyMainUsage::TRANSFER;
  transferQueueFamilyDependencies.queuesCountNeeded = 1;
  transferQueueFamilyDependencies.queuesPriorities = { 1.f, 1.f };
  transferQueueFamilyDependencies.transfer = UTRUE;

  mPhysicalDeviceDependencies.deviceType = EPhysicalDeviceType::DISCRETE;
  mPhysicalDeviceDependencies.deviceTypeFallback = EPhysicalDeviceType::INTEGRATED;
  mPhysicalDeviceDependencies.queueFamilyIndexesCount = 2;
  mPhysicalDeviceDependencies.queueFamilyDependencies = {
      graphicsQueueFamilyDependencies, transferQueueFamilyDependencies
  };

  // WINDOW SURFACE
  mWindowSurfaceDependencies.presentModeCandidates = {
      VK_PRESENT_MODE_MAILBOX_KHR
  };
}


b32 FRenderContextVulkan::init(const FRenderContextSpecification& renderContextSpecs) {
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
    UFATAL("Could not create Vulkan Instance! Check if drivers or Vulkan SDK on"
           "this host is properly installed!");
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

  // We create logical device (dependent only on VkPhysicalDevice) knowing that present
  // on newly created window surface is available
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

  UINFO("Initialized Vulkan Render Context!");
  return UTRUE;
}


void FRenderContextVulkan::terminate() {
  UTRACE("Terminating Vulkan Render Context...");

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


ERenderLibrary FRenderContextVulkan::getLibrary() const {
  return ERenderLibrary::VULKAN;
}


}
