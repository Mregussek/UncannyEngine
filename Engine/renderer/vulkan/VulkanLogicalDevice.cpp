
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


template<typename TProperties> const char* retrievePropertyName(const TProperties& p) {
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


template<typename TProperties>
void iterateOverAndLog(const std::vector<TProperties>& properties,
                       const char* (*pRetrieveFunc)(const TProperties&),
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

  // VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

  UDEBUG("Created vulkan logical device!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeLogicalDevice() {
  UTRACE("Closing vulkan logical device...");

  UDEBUG("Closed vulkan logical device!");
  return UTRUE;
}


}
