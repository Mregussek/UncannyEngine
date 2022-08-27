
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


static b32 createTriangleGraphicsPipeline();


b32 FRenderContextVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  b32 createdTrianglePipeline{ createTriangleGraphicsPipeline() };
  if (not createdTrianglePipeline) {
    UERROR("Could not create triangle graphics pipeline!");
    return UFALSE;
  }

  UDEBUG("Created graphics pipelines general!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsPipelinesGeneral() {
  UTRACE("Closing graphics pipelines general...");

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


b32 createTriangleGraphicsPipeline() {
  UTRACE("Creating triangle graphics pipeline...");

  UDEBUG("Created triangle graphics pipeline!");
  return UTRUE;
}


}
