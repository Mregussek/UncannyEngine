
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  UDEBUG("Created graphics pipelines general!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsPipelinesGeneral() {
  UTRACE("Closing graphics pipelines general...");

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


}
