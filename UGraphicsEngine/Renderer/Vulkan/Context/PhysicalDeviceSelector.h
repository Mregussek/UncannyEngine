
#ifndef UNCANNYENGINE_PHYSICALDEVICESELECTOR_H
#define UNCANNYENGINE_PHYSICALDEVICESELECTOR_H


#include <volk.h>
#include <span>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FPhysicalDeviceSelector is a helper class for selection of proper physical devices.
class FPhysicalDeviceSelector
{
public:

  /// @brief Selects physical device that gets the best score in its own validation.
  /// @param availablePhysicalDevices view array of all available physical devices
  /// @return best physical device
  [[nodiscard]] static VkPhysicalDevice Select(std::span<VkPhysicalDevice> availablePhysicalDevices);

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICESELECTOR_H
