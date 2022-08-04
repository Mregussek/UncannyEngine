
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsQueues() {
  UTRACE("Creating graphics queues for rendering...");

  UDEBUG("Created graphics queues!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsQueues() {
  UTRACE("Closing graphics queues...");

  UDEBUG("Closed graphics queues!");
  return UTRUE;
}


}
