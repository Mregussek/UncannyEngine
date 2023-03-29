
#ifndef UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <span>
#include <vector>
#include "Typedefs.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


/// @brief FLogicalDeviceAttributes is a helper class for FLogicalDevice.
/// @details It is responsible for querying queue families with some internal specification, selecting
/// several device features and managing extensions that logical device will use.
class FLogicalDeviceAttributes
{
public:

  /// @brief Initializes member values for queue family indexes and its queue indexes.
  /// @details Queue family indexes are retrieved from FQueueFamilySelector class. Queue indexes are set in
  /// this method. It is expected that graphics queue index = 0, transfer queue index = 0 and compute queue
  /// index = 0. However it tries to set present index = 1 only if familyProperties.queueCount > 1.
  /// @param familyProperties all queue family properties needed for proper queue family selection
  /// @param vkInstance VkInstance handle
  /// @param vkPhysicalDevice VkPhysicalDevice handle
  void InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> familyProperties,
                                    VkInstance vkInstance,
                                    VkPhysicalDevice vkPhysicalDevice);

  /// @brief Adds requested extension if it is available. FPhysicalDeviceAttributes is needed for
  /// availability validation.
  /// @param extensionName requested extension name, like VK_KHR_SWAPCHAIN_EXTENSION_NAME
  /// @param physicalDeviceAttributes just physical device attributes wrapper class
  /// @returns boolean information, if requested extension is added properly
  b8 AddExtensionName(const char* extensionName, const FPhysicalDeviceAttributes& physicalDeviceAttributes);

  /// @brief Just sets physical device features member value, must be called after added all extensions
  /// @param physicalDeviceAttributes physical device attributes
  void InitializeDeviceFeatures(const FPhysicalDeviceAttributes& physicalDeviceAttributes);

  [[nodiscard]] const std::vector<const char*>& GetRequiredExtensions() const { return m_RequestedExtensions; }
  [[nodiscard]] const VkPhysicalDeviceFeatures2& GetDeviceFeatures2() const { return m_DeviceFeatures2; }

  [[nodiscard]] FQueueFamilyIndex GetGraphicsFamilyIndex() const noexcept { return m_GraphicsQueueFamilyIndex; }
  [[nodiscard]] FQueueFamilyIndex GetPresentFamilyIndex() const noexcept { return m_PresentQueueFamilyIndex; }
  [[nodiscard]] FQueueFamilyIndex GetTransferFamilyIndex() const noexcept { return m_TransferQueueFamilyIndex; }
  [[nodiscard]] FQueueFamilyIndex GetComputeFamilyIndex() const noexcept { return m_ComputeQueueFamilyIndex; }

  [[nodiscard]] FQueueIndex GetGraphicsQueueIndex() const noexcept { return m_GraphicsQueueIndex; }
  [[nodiscard]] FQueueIndex GetPresentQueueIndex() const noexcept { return m_PresentQueueIndex; }
  [[nodiscard]] FQueueIndex GetTransferQueueIndex() const noexcept { return m_TransferQueueIndex; }
  [[nodiscard]] FQueueIndex GetComputeQueueIndex() const noexcept { return m_ComputeQueueIndex; }

private:

  // required extensions...
  std::vector<const char*> m_RequestedExtensions{};

  // Device features...
  VkPhysicalDeviceFeatures2 m_DeviceFeatures2{};
  VkPhysicalDeviceVulkan11Features m_Vulkan11Features{};
  VkPhysicalDeviceVulkan12Features m_Vulkan12Features{};
  VkPhysicalDeviceVulkan13Features m_Vulkan13Features{};
  VkPhysicalDeviceRayTracingPipelineFeaturesKHR m_RayTracingPipelineFeatures{};
  VkPhysicalDeviceAccelerationStructureFeaturesKHR m_AccelerationStructureFeatures{};
  VkPhysicalDeviceRayQueryFeaturesKHR m_RayQueryFeatures{};

  // Device properties...
  VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_RayTracingPipelineProperties{};
  VkPhysicalDeviceAccelerationStructurePropertiesKHR m_AccelerationStructureProperties{};

  // Those are queue family indexes
  FQueueFamilyIndex m_GraphicsQueueFamilyIndex{ UUNUSED };
  FQueueFamilyIndex m_PresentQueueFamilyIndex{ UUNUSED };
  FQueueFamilyIndex m_TransferQueueFamilyIndex{ UUNUSED };
  FQueueFamilyIndex m_ComputeQueueFamilyIndex{ UUNUSED };

  // Those are queue indexes for vkDeviceGetQueue...
  FQueueIndex m_GraphicsQueueIndex{ UUNUSED };
  FQueueIndex m_PresentQueueIndex{ UUNUSED };
  FQueueIndex m_TransferQueueIndex{ UUNUSED };
  FQueueIndex m_ComputeQueueIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H
