
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createRenderImages() {
  UTRACE("Creating Render Target Images...");


  UDEBUG("Created Render Target Images!");
  return UTRUE;
}

b32 FRenderContextVulkan::closeRenderImages() {
  UTRACE("Closing Render Target Images...");


  UDEBUG("Closed Render Target Images!");
  return UTRUE;
}


}
