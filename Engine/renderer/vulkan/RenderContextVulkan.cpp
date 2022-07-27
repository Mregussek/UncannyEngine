
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::init(FRenderContextSpecification renderContextSpecs) {
  UTRACE("Initializing Vulkan Render Context...");
  mSpecs = renderContextSpecs;

  b32 properlyCreatedInstance{ createInstance() };
  if (not properlyCreatedInstance) {
    UFATAL("Could not create Vulkan Instance!");
    return UFALSE;
  }

  b32 properlyCreatedPhysicalDevice{ createPhysicalDevice() };
  if (not properlyCreatedPhysicalDevice) {
    UFATAL("Could not pick Vulkan Physical Device!");
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
