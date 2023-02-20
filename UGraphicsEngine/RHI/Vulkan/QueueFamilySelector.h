
#ifndef UNCANNYENGINE_QUEUEFAMILYSELECTOR_H
#define UNCANNYENGINE_QUEUEFAMILYSELECTOR_H


#include <volk.h>
#include <span>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


class FQueueFamilySelector {
public:

  [[nodiscard]] u32 SelectGraphicsQueueFamily(std::span<VkQueueFamilyProperties> queueFamilyProperties) const;
  [[nodiscard]] u32 SelectPresentQueueFamily(std::span<VkQueueFamilyProperties> queueFamilyProperties) const;
  [[nodiscard]] u32 SelectTransferQueueFamily(std::span<VkQueueFamilyProperties> queueFamilyProperties) const;

};


}


#endif //UNCANNYENGINE_QUEUEFAMILYSELECTOR_H
