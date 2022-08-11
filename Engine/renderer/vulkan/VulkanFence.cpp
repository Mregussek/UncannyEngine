
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsFences() {
  UTRACE("Creating graphics fences...");

  UDEBUG("Created graphics fences!");
  return UFALSE;
}


b32 FRenderContextVulkan::closeGraphicsFences() {
  UTRACE("Closing graphics fences...");

  UDEBUG("Closed graphics fences!");
  return UTRUE;
}


}
