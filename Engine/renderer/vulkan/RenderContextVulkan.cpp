
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::init(FRenderContextSpecification renderContextSpecs) {
  UTRACE("Initializing Vulkan Render Context...");
  mSpecs = renderContextSpecs;

  // Firstly creating instance as it is mandatory for proper Vulkan work...
  b32 properlyCreatedInstance{ createInstance() };
  if (not properlyCreatedInstance) {
    UFATAL("Could not create Vulkan Instance!");
    return UFALSE;
  }

  // Secondly created physical device...
  b32 properlyCreatedPhysicalDevice{ createPhysicalDevice() };
  if (not properlyCreatedPhysicalDevice) {
    UFATAL("Could not pick Vulkan Physical Device!");
    return UFALSE;
  }

  // We can create surface only with instance and check presentation support with
  // physical device that is why third step is window surface creation...
  b32 properlyCreatedWindowSurface{ createWindowSurface() };
  if (not properlyCreatedWindowSurface) {
    UFATAL("Could not create window surface, rendering is not available!");
    return UFALSE;
  }

  UINFO("Initialized Vulkan Render Context!");
  return UTRUE;
}


void FRenderContextVulkan::terminate() {
  UTRACE("Terminating Vulkan Render Context...");

  b32 closedInstance{ closeInstance() };
  if (not closedInstance) {

  }

  UINFO("Terminated Vulkan Render Context!");
}


}
