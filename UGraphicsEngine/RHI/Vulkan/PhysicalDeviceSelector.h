
#ifndef UNCANNYENGINE_PHYSICALDEVICESELECTOR_H
#define UNCANNYENGINE_PHYSICALDEVICESELECTOR_H


#include <volk.h>
#include <vector>
#include <span>


namespace uncanny::vulkan {


class FPhysicalDeviceSelector {
public:

  [[nodiscard]] VkPhysicalDevice Select(std::span<VkPhysicalDevice> availablePhysicalDevices) const;

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICESELECTOR_H
