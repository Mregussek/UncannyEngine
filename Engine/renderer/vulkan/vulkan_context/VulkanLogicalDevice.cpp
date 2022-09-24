
#include "ContextVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <renderer/vulkan/vulkan_renderer/RendererVulkan.h>
#include <utilities/Logger.h>


namespace uncanny
{


static void iterateOverAndLog(const std::vector<VkExtensionProperties>& properties,
                              const char* areMandatory);


static void iterateOverAndLog(const std::vector<const char*>& properties,
                              const char* areMandatory);


static b32 isRequiredPropertyInVector(
    const char* requiredExt, const std::vector<VkExtensionProperties>& availableExt);


static b32 areRequiredExtensionsAvailable(
    VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions);


b32 FRenderContextVulkan::createLogicalDevice() {
  UTRACE("Creating vulkan logical device...");

  // Device Extensions...
  std::vector<const char*> requiredExtensions{};
  FRendererVulkan::getRequiredExtensions(&requiredExtensions);

  // TODO: extensions support is validated during vkCreateDevice and if is no available
  //  return code is VK_ERROR_EXTENSION_NOT_PRESENT, make sure if checking it here is proper
  // Validate Extensions...
  if (not areRequiredExtensionsAvailable(mVkPhysicalDevice, requiredExtensions)) {
    UERROR("Some required instance layers are not available, cannot start vulkan renderer!");
    return UFALSE;
  }
  iterateOverAndLog(requiredExtensions, "Enable Logical Device Extensions");

  // Retrieve all needed info...
  FQueueFamilyDependencies graphicsFamilyDeps{};
  b32 foundGraphicsDeps{
      getQueueFamilyDependencies(EQueueFamilyMainUsage::GRAPHICS,
                                 mPhysicalDeviceDependencies.queueFamilyDependencies,
                                 &graphicsFamilyDeps) };
  if (not foundGraphicsDeps) {
    UERROR("Could not find graphics queue family dependencies!");
    return UFALSE;
  }
  FQueueFamilyDependencies transferFamilyDeps{};
  b32 foundTransferDeps{
      getQueueFamilyDependencies(EQueueFamilyMainUsage::TRANSFER,
                                 mPhysicalDeviceDependencies.queueFamilyDependencies,
                                 &transferFamilyDeps) };
  if (not foundTransferDeps) {
    UERROR("Could not find transfer queue family dependencies!");
    return UFALSE;
  }

  // TODO: make sure if all physical device features are required
  // Querying physical device features and passing it to create info
  VkPhysicalDeviceFeatures physicalDeviceFeatures{};
  vkGetPhysicalDeviceFeatures(mVkPhysicalDevice, &physicalDeviceFeatures);

  if constexpr (not U_VK_DEBUG) {
    UTRACE("As it is release build, disabling robustBufferAccess!");
    physicalDeviceFeatures.robustBufferAccess = UFALSE;
  }

  u32 neededQueueFamiliesCount{ mPhysicalDeviceDependencies.queueFamilyIndexesCount };
  if (neededQueueFamiliesCount <= 1 and mGraphicsQueueFamilyIndex == mTransferQueueFamilyIndex) {
    UERROR("As graphics queue family index match transfer's index, cannot create several queues!");
    return UFALSE;
  }

  // Create as many create infos as needed...
  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoVector(neededQueueFamiliesCount);

  // Create Queue Family for graphics...
  if (not deviceQueueCreateInfoVector.empty()) {
    deviceQueueCreateInfoVector[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfoVector[0].pNext = nullptr;
    deviceQueueCreateInfoVector[0].flags = 0;
    deviceQueueCreateInfoVector[0].queueFamilyIndex = mGraphicsQueueFamilyIndex;
    deviceQueueCreateInfoVector[0].queueCount = graphicsFamilyDeps.queuesCountNeeded;
    deviceQueueCreateInfoVector[0].pQueuePriorities = graphicsFamilyDeps.queuesPriorities.data();
  }

  // Create Queue Family for transfer...
  if (deviceQueueCreateInfoVector.size() >= 2) {
    deviceQueueCreateInfoVector[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfoVector[1].pNext = nullptr;
    deviceQueueCreateInfoVector[1].flags = 0;
    deviceQueueCreateInfoVector[1].queueFamilyIndex = mTransferQueueFamilyIndex;
    deviceQueueCreateInfoVector[1].queueCount = transferFamilyDeps.queuesCountNeeded;
    deviceQueueCreateInfoVector[1].pQueuePriorities = transferFamilyDeps.queuesPriorities.data();
  }

  VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.queueCreateInfoCount = (u32)deviceQueueCreateInfoVector.size();
  createInfo.pQueueCreateInfos = deviceQueueCreateInfoVector.data();
  createInfo.enabledLayerCount = 0;             // deprecated!
  createInfo.ppEnabledLayerNames = nullptr;    // deprecated!
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  createInfo.pEnabledFeatures = &physicalDeviceFeatures;

  VkResult result{ vkCreateDevice(mVkPhysicalDevice, &createInfo, nullptr, &mVkDevice) };
  if (result != VK_SUCCESS) {
    UERROR("Cannot create logical device!");
    return UFALSE;
  }

  UDEBUG("Created vulkan logical device!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeLogicalDevice() {
  UTRACE("Closing vulkan logical device...");

  if (mVkDevice != VK_NULL_HANDLE) {
    UTRACE("Destroying vulkan logical device...");
    vkDeviceWaitIdle(mVkDevice);
    vkDestroyDevice(mVkDevice, nullptr);
  }
  else {
    UWARN("Logical device is not created, so it won't be closed!");
  }

  UDEBUG("Closed vulkan logical device!");
  return UTRUE;
}


void iterateOverAndLog(const std::vector<VkExtensionProperties>& properties,
                       const char* areMandatory) {
  if constexpr (not ENABLE_TERMINAL_LOGGING) {
    return;
  }
  UTRACE("{} PhysicalDevice VkExtensionsProperties:", areMandatory);
  for (const VkExtensionProperties& property : properties) {
    std::cout << property.extensionName << ", ";
  }
  std::cout << '\n';
}


void iterateOverAndLog(const std::vector<const char*>& properties,
                       const char* areMandatory) {
  if constexpr (not ENABLE_TERMINAL_LOGGING) {
    return;
  }
  UTRACE("{} PhysicalDevice:", areMandatory);
  for (const char* property : properties) {
    std::cout << property << ", ";
  }
  std::cout << '\n';
}


b32 isRequiredPropertyInVector(const char* requiredExt,
                               const std::vector<VkExtensionProperties>& availableExt) {
  auto it{ std::find_if(availableExt.begin(), availableExt.end(),
                        [requiredExt](const VkExtensionProperties& properties) {
                          return std::strcmp(properties.extensionName, requiredExt) == 0;
                        })};
  if (it != availableExt.end()) {
    UTRACE("Required extension {} is in available extensions!", requiredExt);
    return UTRUE;
  }

  UTRACE("Required extension {} is not a available extension, cannot find it in vec!", requiredExt);
  return UFALSE;
}


b32 areRequiredExtensionsAvailable(VkPhysicalDevice physicalDevice,
                                             const std::vector<const char*>& requiredExtensions) {
  UTRACE("Ensure all required extensions are available for logical device...");
  u32 extensionCount{ 0 };
  U_VK_ASSERT( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                                    nullptr));
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  U_VK_ASSERT( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                                    availableExtensions.data()) );

  if constexpr (U_VK_DEBUG) {
    iterateOverAndLog(availableExtensions, "AVAILABLE");
  }

  for (const char* required : requiredExtensions) {
    if (not isRequiredPropertyInVector(required, availableExtensions)) {
      UTRACE("Property {} is not available!", required);
      return UFALSE;
    }
  }

  UTRACE("All required device extensions are available!");
  return UTRUE;
}


}
