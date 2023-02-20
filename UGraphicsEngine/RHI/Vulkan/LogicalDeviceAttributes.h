
#ifndef UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <span>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


class FInstance;
class FPhysicalDevice;


class FLogicalDeviceAttributes {
public:

  void InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                    const FInstance& instance,
                                    const FPhysicalDevice& physicalDevice);

  [[nodiscard]] u32 GetGraphicsQueueFamilyIndex() const noexcept { return m_GraphicsQueueFamilyIndex; }
  [[nodiscard]] u32 GetPresentQueueFamilyIndex() const noexcept { return m_PresentQueueFamilyIndex; }
  [[nodiscard]] u32 GetTransferQueueFamilyIndex() const noexcept { return m_TransferQueueFamilyIndex; }

private:

  u32 m_GraphicsQueueFamilyIndex{ UUNUSED };
  u32 m_PresentQueueFamilyIndex{ UUNUSED };
  u32 m_TransferQueueFamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICEATTRIBUTES_H
