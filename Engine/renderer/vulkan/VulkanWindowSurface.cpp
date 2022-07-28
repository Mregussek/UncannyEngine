
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny
{


b32 FRenderContextVulkan::createWindowSurface() {
  UTRACE("Creating window surface...");
  FWindow* pWindow{ mSpecs.pWindow }; // wrapper

  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    UTRACE("Creating Win32 KHR window surface...");
    VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    createInfo.hinstance = GetModuleHandle(nullptr);
    if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
      FWindowGLFW* pWindowGLFW{ dynamic_cast<FWindowGLFW*>(pWindow) };
      createInfo.hwnd = pWindowGLFW->getWindowHandle();
    }
    auto vkCreateWin32SurfaceKHR =
        (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(mVkInstance, "vkCreateWin32SurfaceKHR");
    U_VK_ASSERT( vkCreateWin32SurfaceKHR(mVkInstance, &createInfo, nullptr, &mVkWindowSurface) );
  }

  UDEBUG("Created window surface!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeWindowSurface() {
  UTRACE("Closing Window Surface...");

  vkDestroySurfaceKHR(mVkInstance, mVkWindowSurface, nullptr);

  UDEBUG("Closed Window Surface!");
  return UTRUE;
}


b32 windowSurfaceSupportVulkanAPI(FWindow* pWindow) {
  if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
    i32 isVulkanSupportedByGLFW{ glfwVulkanSupported() };
    if (isVulkanSupportedByGLFW == GLFW_FALSE) {
      UERROR("GLFW does not even minimally support Vulkan API!");
      return UFALSE;
    }

    UTRACE("Current window {} supports vulkan!", pWindow->getSpecs().name);
    return UTRUE;
  }

  UERROR("Current window {} does not support vulkan renderer!", pWindow->getSpecs().name);
  return UFALSE;
}


void getRequiredWindowSurfaceInstanceExtensions(
    FWindow* pWindow, std::vector<const char*>* pRequiredExtensions) {
  if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
    UTRACE("Adding Window GLFW surface extensions to VkInstance...");
    u32 extensionsCountGLFW{ 0 };
    const char** requiredExtensionsGLFW{ glfwGetRequiredInstanceExtensions(&extensionsCountGLFW) };

    for (u32 i = 0; i < extensionsCountGLFW; i++) {
      pRequiredExtensions->push_back(requiredExtensionsGLFW[i]);
    }
    return;
  }

  UERROR("Unknown window {} library, cannot retrieve required vulkan instance extensions!",
         pWindow->getSpecs().name);
}


b32 windowSurfaceSupportPresentationOnPhysicalDevice(
    FWindow* pWindow, VkInstance instance, VkPhysicalDevice physicalDevice, u32 queueFamilyIndex) {
  if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
    i32 supported = glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice,
                                                             queueFamilyIndex);
    if (supported == GLFW_TRUE) {
      UTRACE("Presentation supported for WindowSurface on physical device and queue family index!");
      return UTRUE;
    }

    UERROR("WindowSurface does not support presentation on physical device and queue family index");
    return UFALSE;
  }

  UERROR("Unknown window {} library, cannot retrieve required vulkan instance extensions!",
         pWindow->getSpecs().name);
  return UFALSE;
}


}
