
#ifndef UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FPhysicalDeviceAttributes is a helper class for FPhysicalDevice. It acquires all necessary
/// information about device and provides getter methods for those. Also it validates extension availability.
/// @details It is used also in FPhysicalDeviceSelector as it is written in a way that, it only retrieves
/// information about physical device. It is used for retrieving information about device extensions,
/// properties, queue families etc.
class FPhysicalDeviceAttributes
{
public:

  FPhysicalDeviceAttributes() = default;

  FPhysicalDeviceAttributes(const FPhysicalDeviceAttributes& other) = delete;
  FPhysicalDeviceAttributes(FPhysicalDeviceAttributes&& other) = delete;

  FPhysicalDeviceAttributes& operator=(const FPhysicalDeviceAttributes& other) = delete;
  FPhysicalDeviceAttributes& operator=(FPhysicalDeviceAttributes&& other) = delete;

public:

  /// @brief Initializes whole attributes class and collects all necessary information
  /// @param vkPhysicalDevice physical device for what this information is needed
  void Initialize(VkPhysicalDevice vkPhysicalDevice);

  /// @brief Validates if extension name is available on the physical device
  /// @param extensionName extension name, like VK_KHR_SWAPCHAIN_EXTENSION_NAME
  /// @returns boolean information if extension is available
  b8 IsExtensionPresent(const char* extensionName) const;

  /// @brief Queries specific properties like VkPhysicalDeviceRayTracingPipelinePropertiesKHR
  /// @tparam TProperties queried Vulkan structure
  /// @param pProperties pointer to queried vulkan structure, where result will be stored
  template<typename TProperties> void QueryProperties2(TProperties* pProperties) const;

  /// @brief Queries specific features like VkPhysicalDeviceRayTracingPipelineFeaturesKHR
  /// @tparam TProperties queried Vulkan structure
  /// @param pProperties pointer to queried vulkan structure, where result will be stored
  template<typename TFeatures> void QueryFeatures2(TFeatures* pFeatures) const;

// Getters
public:

  [[nodiscard]] const VkPhysicalDeviceProperties& GetDeviceProperties() const { return m_Properties; }
  [[nodiscard]] const VkPhysicalDeviceFeatures& GetDeviceFeatures() const { return m_Features; }
  [[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
  [[nodiscard]] const std::vector<VkExtensionProperties>& GetExtensionProperties() const { return m_ExtensionProperties; }
  [[nodiscard]] const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_QueueFamilyProperties; }

private:

  std::vector<VkExtensionProperties> m_ExtensionProperties{};
  std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties{};
  VkPhysicalDeviceProperties m_Properties{};
  VkPhysicalDeviceFeatures m_Features{};
  VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };

};


}


#include "PhysicalDeviceAttributes.inl"


#endif //UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
