
#include "PhysicalDevice.h"
#include <utilities/Logger.h>


namespace uncanny::vkf {


b32 FPhysicalDeviceVulkan::init() {
  UTRACE("Initializing physical device vulkan...");

  UDEBUG("Initialized physical device vulkan...");
  return UTRUE;
}


void FPhysicalDeviceVulkan::terminate() {
  UTRACE("Terminating physical device vulkan...");

  UDEBUG("Terminated physical device vulkan...");
}


}
