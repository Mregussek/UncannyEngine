
#ifndef UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FPhysicalDeviceAttributes is a helper class for FPhysicalDevice. It acquires all necessary
/// information about device and provides getter methods for those. Also it validates extension availability.
/// @details It is used also  in FPhysicalDeviceSelector as it is written in a way that, it only retrieves
/// information about physical device. It is used for retrieving information about device extensions,
/// properties, queue families etc.
class FPhysicalDeviceAttributes
{
public:

  /// @brief Initializes whole attributes class and collects all necessary information
  /// @param vkPhysicalDevice physical device for what this information is needed
  void Initialize(VkPhysicalDevice vkPhysicalDevice);

  /// @brief Validates if extension name is available on the physical device
  /// @param extensionName extension name, like VK_KHR_SWAPCHAIN_EXTENSION_NAME
  /// @returns boolean information if extension is available
  b8 IsExtensionPresent(const char* extensionName) const;

  [[nodiscard]] const VkPhysicalDeviceProperties& GetDeviceProperties() const { return m_Properties; }
  [[nodiscard]] const VkPhysicalDeviceFeatures& GetDeviceFeatures() const { return m_Features; }
  [[nodiscard]] const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return m_MemoryProperties; }
  [[nodiscard]] const std::vector<VkExtensionProperties>& GetExtensionProperties() const { return m_ExtensionProperties; }
  [[nodiscard]] const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_QueueFamilyProperties; }

  template<typename TProperties>
  void QueryProperties2(TProperties* pProperties) const
  {
    VkPhysicalDeviceProperties2 deviceProperties2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext = static_cast<void*>(pProperties),
    };
    vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &deviceProperties2);
  }

  template<typename TFeatures>
  void QueryFeatures2(TFeatures* pFeatures) const
  {
    VkPhysicalDeviceFeatures2 deviceFeatures2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = static_cast<void*>(pFeatures),
    };
    vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &deviceFeatures2);
  }

private:

  VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
  VkPhysicalDeviceProperties m_Properties{};
  VkPhysicalDeviceFeatures m_Features{};
  VkPhysicalDeviceMemoryProperties m_MemoryProperties{};
  std::vector<VkExtensionProperties> m_ExtensionProperties{};
  std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties{};

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
