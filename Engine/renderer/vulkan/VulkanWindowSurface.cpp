
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny
{


b32 FRenderContextVulkan::windowSurfaceSupportVulkan() const {
  FWindow* pWindow{ mSpecs.pWindow }; // wrapper
  if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
    i32 isVulkanSupportedByGLFW{ glfwVulkanSupported() };
    if (not isVulkanSupportedByGLFW) {
      UERROR("GLFW does not even minimally support Vulkan API!");
      return UFALSE;
    }

    UTRACE("Current window {} supports vulkan!", pWindow->getSpecs().name);
    return UTRUE;
  }

  UERROR("Current window {} does not support vulkan renderer!", pWindow->getSpecs().name);
  return UFALSE;
}


void FRenderContextVulkan::getRequiredWindowSurfaceExtensions(
    std::vector<const char*>* pRequiredExtensions) const {
  UTRACE("Adding window surface extensions to VkInstance...");
  u32 extensionsCountGLFW{ 0 };
  const char** requiredExtensionsGLFW{ glfwGetRequiredInstanceExtensions(&extensionsCountGLFW) };

  for (u32 i = 0; i < extensionsCountGLFW; i++) {
    pRequiredExtensions->push_back(requiredExtensionsGLFW[i]);
  }
}


b32 FRenderContextVulkan::createWindowSurface() {
  UTRACE("Creating window surface...");
  FWindow* pWindow{ mSpecs.pWindow }; // wrapper



  UDEBUG("Created window surface!");
  return UTRUE;
}


}
