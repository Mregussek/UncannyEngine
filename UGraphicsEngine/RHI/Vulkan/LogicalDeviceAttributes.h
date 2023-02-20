
#ifndef UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <span>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


class FPhysicalDeviceAttributes;


class FLogicalDeviceAttributes {
public:

  void InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                    VkInstance vkInstance,
                                    VkPhysicalDevice vkPhysicalDevice);

  b8 AddExtensionName(const char* extensionName, const FPhysicalDeviceAttributes& physicalDeviceAttributes);

  [[nodiscard]] u32 GetGraphicsQueueFamilyIndex() const noexcept { return m_GraphicsQueueFamilyIndex; }
  [[nodiscard]] u32 GetPresentQueueFamilyIndex() const noexcept { return m_PresentQueueFamilyIndex; }
  [[nodiscard]] u32 GetTransferQueueFamilyIndex() const noexcept { return m_TransferQueueFamilyIndex; }

private:

  std::vector<const char*> m_RequestedExtensions{};

  u32 m_GraphicsQueueFamilyIndex{ UUNUSED };
  u32 m_PresentQueueFamilyIndex{ UUNUSED };
  u32 m_TransferQueueFamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H
