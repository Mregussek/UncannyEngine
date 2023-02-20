
#include "PhysicalDeviceSelector.h"


namespace uncanny::vulkan {


VkPhysicalDevice FPhysicalDeviceSelector::Select(std::span<VkPhysicalDevice> availablePhysicalDevices) const {
  return availablePhysicalDevices[0];
}


}
