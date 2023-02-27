
#ifndef UNCANNYENGINE_PHYSICALDEVICESELECTOR_H
#define UNCANNYENGINE_PHYSICALDEVICESELECTOR_H


#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include <vector>
#include <span>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


typedef i32 FPhysicalDeviceScore;


/*
 * @brief FPhysicalDeviceSelector is a helper class for selection of proper physical devices.
 */
class FPhysicalDeviceSelector {
public:

  [[nodiscard]] VkPhysicalDevice Select(std::span<VkPhysicalDevice> availablePhysicalDevices) const;

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICESELECTOR_H
