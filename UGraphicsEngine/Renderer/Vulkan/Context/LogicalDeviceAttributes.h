
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


/*
 * @brief FLogicalDeviceAttributes is a helper class for FLogicalDevice. It is responsible for querying
 * queue families with some internal specification, selecting several device features and managing
 * extensions that logical device will use.
 * it has many getter methods for retrieval information during runtime for Render Device.
 */
class FLogicalDeviceAttributes
{
public:

  void InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> familyProperties,
                                    VkInstance vkInstance,
                                    VkPhysicalDevice vkPhysicalDevice);

  void InitializeDeviceFeatures(const VkPhysicalDeviceFeatures& physicalDeviceFeatures);

  b8 AddExtensionName(const char* extensionName, const FPhysicalDeviceAttributes& physicalDeviceAttributes);

  [[nodiscard]] FQueueFamilyIndex GetGraphicsQueueFamilyIndex() const noexcept { return m_GraphicsQueueFamilyIndex; }
  [[nodiscard]] FQueueFamilyIndex GetPresentQueueFamilyIndex() const noexcept { return m_PresentQueueFamilyIndex; }
  [[nodiscard]] FQueueFamilyIndex GetTransferQueueFamilyIndex() const noexcept { return m_TransferQueueFamilyIndex; }
  [[nodiscard]] FQueueFamilyIndex GetComputeQueueFamilyIndex() const noexcept { return m_ComputeQueueFamilyIndex; }

  [[nodiscard]] FQueueIndex GetGraphicsQueueIndex() const noexcept { return m_GraphicsQueueIndex; }
  [[nodiscard]] FQueueIndex GetPresentQueueIndex() const noexcept { return m_PresentQueueIndex; }
  [[nodiscard]] FQueueIndex GetTransferQueueIndex() const noexcept { return m_TransferQueueIndex; }
  [[nodiscard]] FQueueIndex GetComputeQueueIndex() const noexcept { return m_ComputeQueueIndex; }

  [[nodiscard]] const std::vector<const char*>& GetRequiredExtensions() const { return m_RequestedExtensions; }
  [[nodiscard]] const VkPhysicalDeviceFeatures& GetDeviceFeatures() const { return m_DeviceFeatures; }

private:

  std::vector<const char*> m_RequestedExtensions{};
  VkPhysicalDeviceFeatures m_DeviceFeatures{};

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
