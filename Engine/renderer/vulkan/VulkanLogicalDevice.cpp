
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


template<typename TProperties> static const char* retrievePropertyName(const TProperties& p) {
  if constexpr (is_vk_extension_properties<TProperties>::value) {
    return p.extensionName;
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    return p.layerName;
  }
  else if constexpr (std::is_same<TProperties, const char*>::value) {
    return p;
  }
  else {
    return "Unknown";
  }
}


template<typename TProperties> static void iterateOverAndLog(
    const std::vector<TProperties>& properties, const char* (*pRetrieveFunc)(const TProperties&),
    const char* areMandatory) {
  UTRACE("{} PhysicalDevice {} properties:", areMandatory, typeid(TProperties).name());
  for (const TProperties& property : properties) {
    std::cout << pRetrieveFunc(property) << ", ";
  }
  std::cout << '\n';
}


b32 FRenderContextVulkan::createLogicalDevice() {
  UTRACE("Creating vulkan logical device...");

  uint32_t layerCount{ 0 };
  U_VK_ASSERT( vkEnumerateDeviceLayerProperties(mVkPhysicalDevice, &layerCount, nullptr) );
  std::vector<VkLayerProperties> availableLayers(layerCount);
  U_VK_ASSERT( vkEnumerateDeviceLayerProperties(mVkPhysicalDevice, &layerCount,
                                                availableLayers.data()) );

  uint32_t extensionCount{ 0 };
  U_VK_ASSERT( vkEnumerateDeviceExtensionProperties(mVkPhysicalDevice, nullptr, &extensionCount,
                                                    nullptr));
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  U_VK_ASSERT( vkEnumerateDeviceExtensionProperties(mVkPhysicalDevice, nullptr, &extensionCount,
                                                    availableExtensions.data()) );

  iterateOverAndLog(availableLayers, retrievePropertyName, "AVAILABLE");
  iterateOverAndLog(availableExtensions, retrievePropertyName, "AVAILABLE");

  std::vector<VkDeviceQueueCreateInfo> deviceQueueInfoVector(
      mSpecs.physicalDeviceDependencies.queueFamilyIndexesCount);

  for (u32 i = 0; i < deviceQueueInfoVector.size(); i++) {
    const FQueueFamilyDependencies& queueDependencies{
      mSpecs.physicalDeviceDependencies.pQueueFamilyDependencies[i] };

    deviceQueueInfoVector[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueInfoVector[i].pNext = nullptr;
    deviceQueueInfoVector[i].flags = VK_FALSE;
    deviceQueueInfoVector[i].queueFamilyIndex = mQueueFamilyIndexVector[i];
    deviceQueueInfoVector[i].queueCount = queueDependencies.queuesCountNeeded;
    deviceQueueInfoVector[i].pQueuePriorities = queueDependencies.pQueuePriorities;
  }

  VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = VK_FALSE;
  createInfo.queueCreateInfoCount = (u32)deviceQueueInfoVector.size();
  createInfo.pQueueCreateInfos = deviceQueueInfoVector.data();
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;
  createInfo.enabledExtensionCount = 0;
  createInfo.ppEnabledExtensionNames = nullptr;
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

  vkDestroyDevice(mVkDevice, nullptr);

  UDEBUG("Closed vulkan logical device!");
  return UTRUE;
}


}
