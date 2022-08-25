
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createRenderPasses() {
  UTRACE("Creating render passes...");

  UDEBUG("Created render passes!");
  return UTRUE;
}

b32 FRenderContextVulkan::closeRenderPasses() {
  UTRACE("Closing render passes...");

  UDEBUG("Closed render passes!");
  return UTRUE;
}



}
