
#include "ContextVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny
{


static VkExtent2D getProperExtent2D(const VkSurfaceCapabilitiesKHR& capabilities,
                                    const FWindow* pWindow);


static VkSurfaceFormatKHR getProperSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats,
    const std::vector<VkSurfaceFormatKHR>& formatCandidates);


static VkPresentModeKHR getProperPresentModeKHR(
    const std::vector<VkPresentModeKHR>& availableModes,
    const std::vector<VkPresentModeKHR>& modeCandidates);


#ifdef VK_USE_PLATFORM_WIN32_KHR
static HWND retrieveHandleFromWindow(const FWindow* pWindow) {
  if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
    const FWindowGLFW* pWindowGLFW{ dynamic_cast<const FWindowGLFW*>(pWindow) };
    return pWindowGLFW->getWindowHandle();
  }

  UFATAL("Could not retrieve window handle!");
  return {};
}
#endif


b32 FRenderContextVulkan::createWindowSurface() {
  UTRACE("Creating window surface...");

  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    UTRACE("Creating Win32 KHR window surface...");
    VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = GetModuleHandle(nullptr);
    createInfo.hwnd = retrieveHandleFromWindow(mSpecs.pWindow);
    auto vkCreateWin32SurfaceKHR =
        (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(mVkInstance, "vkCreateWin32SurfaceKHR");
    VkResult result = vkCreateWin32SurfaceKHR(mVkInstance, &createInfo, nullptr, &mVkWindowSurface);
    if (result != VK_SUCCESS) {
      UERROR("Could not create win32 window surface! Cannot present anything to the screen!");
      return UFALSE;
    }
  }

  // query if physical device with queue family and created surface supports presentation
  VkBool32 supportedPresentation{ VK_FALSE };
  vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysicalDevice, mGraphicsQueueFamilyIndex,
                                       mVkWindowSurface, &supportedPresentation);
  if (not supportedPresentation) {
    UERROR("Presentation is not supported in newly created window surface!");
    return UFALSE;
  }

  // when window surface is created we can query information about it...
  b32 collectedCapabilities{ collectWindowSurfaceCapabilities() };
  if (not collectedCapabilities) {
    UERROR("Could not collect window surface capabilities!");
    return UFALSE;
  }

  UDEBUG("Created window surface!");
  return UTRUE;
}


b32 FRenderContextVulkan::collectWindowSurfaceCapabilities() {
  UTRACE("Collecting window surface capabilities...");

  // query surface capabilities info...
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &mVkSurfaceCapabilities) );

  // query image extent info...
  mVkSurfaceExtent2D = getProperExtent2D(mVkSurfaceCapabilities, mSpecs.pWindow);
  UTRACE("Found surface extent width {} height {}",
         mVkSurfaceExtent2D.width, mVkSurfaceExtent2D.height);

  // query present modes info...
  u32 presentCount{ 0 };
  U_VK_ASSERT( vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &presentCount, nullptr) );
  std::vector<VkPresentModeKHR> availablePresentModes(presentCount);
  U_VK_ASSERT( vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &presentCount,
                                                         availablePresentModes.data()) );

  mVkPresentMode = getProperPresentModeKHR(availablePresentModes,
                                           mWindowSurfaceDependencies.presentModeCandidates);
  UTRACE("Found surface present mode {}", mVkPresentMode);

  UDEBUG("Collected window surface capabilities!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeWindowSurface() {
  UTRACE("Closing Window Surface...");

  if (mVkWindowSurface != VK_NULL_HANDLE) {
    UTRACE("Destroying window image surface...");
    vkDestroySurfaceKHR(mVkInstance, mVkWindowSurface, nullptr);
  }
  else {
    UWARN("Window surface is not created, so it won't be closed!");
  }

  UDEBUG("Closed Window Surface!");
  return UTRUE;
}


b32 FRenderContextVulkan::isWindowSurfacePresentableImageExtentProper() {
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &mVkSurfaceCapabilities) );

  const u32 surfaceWidth{ mVkSurfaceCapabilities.currentExtent.width };
  const u32 surfaceHeight{ mVkSurfaceCapabilities.currentExtent.height };

  if (surfaceWidth <= 1 and surfaceHeight <= 1) {
    return UFALSE;
  }

  return UTRUE;
}


b32 FRenderContextVulkan::detectSupportedImageFormatByWindowSurface(
    const std::vector<VkSurfaceFormatKHR>& candidates, VkSurfaceFormatKHR* pOutFormat) {
  UTRACE("Detecting supported image format by window surface from candidates...");

  // query surface format info...
  u32 formatCount{ 0 };
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                    &formatCount, nullptr) );
  std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                    &formatCount, availableFormats.data()) );

  auto it = std::find_if(candidates.cbegin(), candidates.cend(),
                         [availableFormats = std::as_const(availableFormats)]
                             (VkSurfaceFormatKHR candidate) -> b32 {
                           for (VkSurfaceFormatKHR af : availableFormats) {
                             if (candidate.format ==  af.format and candidate.colorSpace == af.colorSpace) {
                               return UTRUE;
                             }
                           }
                           return UFALSE;
                         });
  if (it != candidates.cend()) {
    pOutFormat->format = it->format;
    pOutFormat->colorSpace = it->colorSpace;
    UDEBUG("Found candidate format supported by window surface! f {} cs {}",
           pOutFormat->format, pOutFormat->colorSpace);
    return UTRUE;
  }

  UERROR("Could not find proper format candidate supported by window surface!");
  *pOutFormat = {};
  return UFALSE;
}

b32 windowSurfaceSupportVulkanAPI(const FWindow* pWindow) {
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
    const FWindow* pWindow, std::vector<const char*>* pRequiredExtensions) {
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
    const FWindow* pWindow, VkInstance instance, VkPhysicalDevice physicalDevice,
    u32 queueFamilyIndex) {
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


VkExtent2D getProperExtent2D(const VkSurfaceCapabilitiesKHR& capabilities, const FWindow* pWindow) {
  UTRACE("Looking for proper extent 2D...");

  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    UTRACE("Returning current extent!");
    return capabilities.currentExtent;
  }

  const FWindowSpecification& windowSpecs{ pWindow->getSpecs() };

  VkExtent2D actualExtent{};
  actualExtent.width = (u32)windowSpecs.width;
  actualExtent.height = (u32)windowSpecs.height;

  actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                  capabilities.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                   capabilities.maxImageExtent.height);

  UTRACE("Retrieved actual extent from window and clamped it to min/max values!");
  return actualExtent;
}


VkSurfaceFormatKHR getProperSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats,
    const std::vector<VkSurfaceFormatKHR>& formatCandidates) {
  UTRACE("Looking for supported window surface format...");

  auto it = std::find_if(formatCandidates.cbegin(), formatCandidates.cend(),
                         [availableFormats = std::as_const(availableFormats)]
                         (VkSurfaceFormatKHR candidate) -> b32 {
    for (VkSurfaceFormatKHR af : availableFormats) {
      if (candidate.format ==  af.format and candidate.colorSpace == af.colorSpace) {
        return UTRUE;
      }
    }

    return UFALSE;
  });
  if (it != formatCandidates.cend()) {
    UTRACE("Found format proper candidate!");
    return *it;
  }

  UERROR("As format candidates are not available, returning undefined format!");
  return { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
}


VkPresentModeKHR getProperPresentModeKHR(
    const std::vector<VkPresentModeKHR>& availableModes,
    const std::vector<VkPresentModeKHR>& modeCandidates) {
  UTRACE("Looking for supported present modes for window surface...");

  auto it = std::find_if(modeCandidates.cbegin(), modeCandidates.cend(),
                         [availableModes = std::as_const(availableModes)]
                         (VkPresentModeKHR candidate) -> b32 {
    for (VkPresentModeKHR am : availableModes) {
      if (candidate == am) {
        return UTRUE;
      }
    }

    return UFALSE;
  });
  if (it != modeCandidates.cend()) {
    UTRACE("Found present mode proper candidate!");
    return *it;
  }

  UWARN("As present modes candidates are not available, returning fallback supported!");
  return VK_PRESENT_MODE_FIFO_KHR;
}


}
