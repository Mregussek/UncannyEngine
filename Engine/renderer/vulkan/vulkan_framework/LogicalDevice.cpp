
#include "LogicalDevice.h"
#include <utilities/Logger.h>


namespace uncanny::vkf {


b32 FLogicalDeviceVulkan::init(const FLogicalDeviceInitDependenciesVulkan& deps) {
  UTRACE("Initializing logical device vulkan...");

  UDEBUG("Initialized logical device vulkan!");
  return UTRUE;
}


void FLogicalDeviceVulkan::terminate() {
  UTRACE("Terminating logical device vulkan...");

  UDEBUG("Terminated logical device vulkan!");
}


}
