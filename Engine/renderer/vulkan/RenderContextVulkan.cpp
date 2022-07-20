
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FRenderContextVulkan::init(FRenderContextSpecification renderContextSpecs) {
  UTRACE("Initializing Vulkan Render Context...");

  mSpecs = renderContextSpecs;

  createInstance();
  createPhysicalDevice();

  UINFO("Initialized Vulkan Render Context!");
}


void FRenderContextVulkan::terminate() {
  UTRACE("Terminating Vulkan Render Context...");

  closeInstance();

  UINFO("Terminated Vulkan Render Context!");
}


}
