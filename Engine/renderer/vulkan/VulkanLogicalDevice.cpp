
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanSwapchain.h"
#include <utilities/Logger.h>


namespace uncanny
{


static void iterateOverAndLog(const std::vector<VkExtensionProperties>& properties,
                              const char* areMandatory);


static void iterateOverAndLog(const std::vector<const char*>& properties,
                              const char* areMandatory);


static b32 isRequiredPropertyInVector(
    const char* requiredExt, const std::vector<VkExtensionProperties>& availableExt);


static void ensureAllRequiredExtensionsAreAvailable(
    VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions);


b32 FRenderContextVulkan::createLogicalDevice() {
  UTRACE("Creating vulkan logical device...");

  std::vector<const char*> requiredExtensions{};
  getRequiredSwapchainExtensions(&requiredExtensions);

  ensureAllRequiredExtensionsAreAvailable(mVkPhysicalDevice, requiredExtensions);
  iterateOverAndLog(requiredExtensions, "Enable Logical Device Extensions");

  std::vector<VkDeviceQueueCreateInfo> deviceQueueInfoVector(
      mPhysicalDeviceDependencies.queueFamilyIndexesCount);

  for (u32 i = 0; i < deviceQueueInfoVector.size(); i++) {
    const FQueueFamilyDependencies& queueDependencies{
      mPhysicalDeviceDependencies.queueFamilyDependencies[i] };

    deviceQueueInfoVector[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueInfoVector[i].pNext = nullptr;
    deviceQueueInfoVector[i].flags = VK_FALSE;
    deviceQueueInfoVector[i].queueFamilyIndex = mQueueFamilyVector[i].index;
    deviceQueueInfoVector[i].queueCount = queueDependencies.queuesCountNeeded;
    deviceQueueInfoVector[i].pQueuePriorities = queueDependencies.queuesPriorities.data();
  }

  VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.queueCreateInfoCount = (u32)deviceQueueInfoVector.size();
  createInfo.pQueueCreateInfos = deviceQueueInfoVector.data();
  createInfo.enabledLayerCount = 0;             // deprecated!
  createInfo.ppEnabledLayerNames = nullptr;    // deprecated!
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  createInfo.pEnabledFeatures = &mVkPhysicalDeviceFeatures;

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

  if (mVkDevice == VK_NULL_HANDLE) {
    UWARN("Logical device is not created, so it won't be closed!");
    return UTRUE;
  }

  vkDestroyDevice(mVkDevice, nullptr);

  UDEBUG("Closed vulkan logical device!");
  return UTRUE;
}


void iterateOverAndLog(const std::vector<VkExtensionProperties>& properties,
                       const char* areMandatory) {
  UTRACE("{} PhysicalDevice VkExtensionsProperties:", areMandatory);
  for (const VkExtensionProperties& property : properties) {
    std::cout << property.extensionName << ", ";
  }
  std::cout << '\n';
}


void iterateOverAndLog(const std::vector<const char*>& properties,
                       const char* areMandatory) {
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
                          return std::strcmp(properties.extensionName, requiredExt);
                        })};
  if (it != availableExt.end()) {
    return UTRUE;
  }

  return UFALSE;
}


void ensureAllRequiredExtensionsAreAvailable(VkPhysicalDevice physicalDevice,
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
    }
  }
}


}
