
#ifndef UNCANNYENGINE_QUEUEFAMILYSELECTOR_H
#define UNCANNYENGINE_QUEUEFAMILYSELECTOR_H


#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include <span>
#include <optional>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


class FInstance;
class FPhysicalDevice;


typedef i32 FQueueFamilyScore;


class FQueueFamilySelector {
public:

  [[nodiscard]] std::optional<u32> SelectGraphicsQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                             VkInstance vkInstance,
                                                             VkPhysicalDevice vkPhysicalDevice) const;

  [[nodiscard]] std::optional<u32> SelectPresentQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                            VkInstance vkInstance,
                                                            VkPhysicalDevice vkPhysicalDevice) const;

  [[nodiscard]] std::optional<u32> SelectTransferQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                             VkInstance vkInstance,
                                                             VkPhysicalDevice vkPhysicalDevice) const;

};


}


#endif //UNCANNYENGINE_QUEUEFAMILYSELECTOR_H
