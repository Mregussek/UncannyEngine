
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny
{


b32 FRenderContextVulkan::createWindowSurface() {
  UTRACE("Creating window surface...");
  FWindow* pWindow{ mSpecs.pWindow }; // wrapper



  UDEBUG("Created window surface!");
  return UTRUE;
}


}
