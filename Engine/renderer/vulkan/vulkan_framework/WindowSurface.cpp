
#include "WindowSurface.h"
#include "Utilities.h"
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny::vkf
{


#ifdef WIN32
static HWND retrieveHandleFromWindow(const FWindow* pWindow) {
  if (pWindow->getLibrary() == EWindowLibrary::GLFW) {
    const FWindowGLFW* pWindowGLFW{ dynamic_cast<const FWindowGLFW*>(pWindow) };
    return pWindowGLFW->getWindowHandle();
  }

  UFATAL("Could not retrieve window handle!");
  AssertResultVulkan(VK_ERROR_UNKNOWN);
  return {};
}
#endif


static VkExtent2D getProperExtent2D(const VkSurfaceCapabilitiesKHR& capabilities,
                                    const FWindow* pWindow);

static VkPresentModeKHR getProperPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);


b32 FWindowSurfaceVulkan::init(const FWindowSurfaceInitDependenciesVulkan& deps) {
  UTRACE("Initializing window surface vulkan...");

  m_pWindow = deps.pWindow;

  if constexpr (WIN32) {
    UTRACE("Creating Win32 KHR window surface...");

    auto vkCreateWin32SurfaceKHR =
        (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(deps.instance, "vkCreateWin32SurfaceKHR");

    VkWin32SurfaceCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = GetModuleHandle(nullptr);
    createInfo.hwnd = retrieveHandleFromWindow(deps.pWindow);

    VkResult result = vkCreateWin32SurfaceKHR(deps.instance, &createInfo, nullptr, &m_VkSurface);
    if (result != VK_SUCCESS) {
      UERROR("Could not create win32 window surface! Cannot present anything to the screen!");
      return UFALSE;
    }
  }

  UTRACE("Querying if physical device with queue family and newly created window surface support presentation...");
  VkBool32 supportedPresentation{ VK_FALSE };
  vkGetPhysicalDeviceSurfaceSupportKHR(deps.physicalDevice, deps.queueFamilyIndexGraphics,
                                       m_VkSurface, &supportedPresentation);
  if (not supportedPresentation) {
    UERROR("Presentation is not supported in newly created window surface!");
    return UFALSE;
  }

  updateCapabilities(deps.physicalDevice);

  UDEBUG("Initialized window surface vulkan!");
  return UTRUE;
}


void FWindowSurfaceVulkan::terminate(VkInstance instance) {
  UTRACE("Terminating window surface vulkan...");

  if (m_VkSurface != VK_NULL_HANDLE) {
    UTRACE("Destroying window surface...");
    vkDestroySurfaceKHR(instance, m_VkSurface, nullptr);
  }
  else {
    UWARN("Window surface is not created, so it won't be closed!");
  }

  m_VkSurfaceCaps = {};
  m_VkExtent2D = {};
  m_VkPresentMode = {};

  UDEBUG("Terminated window surface vulkan!");
}


void FWindowSurfaceVulkan::updateCapabilities(VkPhysicalDevice physicalDevice) {
  UTRACE("Querying surface capabilities...");
  AssertResultVulkan( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_VkSurface, &m_VkSurfaceCaps) );

  m_VkExtent2D = getProperExtent2D(m_VkSurfaceCaps, m_pWindow);
  m_VkPresentMode = getProperPresentMode(physicalDevice, m_VkSurface);
}


b32 FWindowSurfaceVulkan::detectSupportedImageFormat(VkPhysicalDevice physicalDevice,
                                                     const std::vector<VkSurfaceFormatKHR>& candidates,
                                                     VkSurfaceFormatKHR* pOutFormat) {
  UTRACE("Detecting supported image format by window surface from candidates...");

  // query surface format info...
  u32 formatCount{ 0 };
  AssertResultVulkan( vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_VkSurface, &formatCount, nullptr) );
  std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
  AssertResultVulkan( vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_VkSurface, &formatCount,
                                                           availableFormats.data()) );

  auto it = std::find_if(candidates.cbegin(), candidates.cend(),
                         [availableFormats = std::as_const(availableFormats)](VkSurfaceFormatKHR candidate) -> b32 {
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


b32 FWindowSurfaceVulkan::isMinimized(VkPhysicalDevice physicalDevice) {
  updateCapabilities(physicalDevice);

  if (m_VkExtent2D.width <= 1 and m_VkExtent2D.height <= 1) {
    return UFALSE;
  }

  return UTRUE;
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


VkPresentModeKHR getProperPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  UTRACE("Querying surface present modes...");
  u32 presentCount{ 0 };
  AssertResultVulkan( vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, nullptr) );
  std::vector<VkPresentModeKHR> presentModes(presentCount);
  AssertResultVulkan( vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount,
                                                                presentModes.data()) );

  for (VkPresentModeKHR presentMode : presentModes) {
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      UTRACE("Returning MAILBOX surface present mode!");
      return VK_PRESENT_MODE_MAILBOX_KHR;
    }
  }

  UTRACE("Returning backup FIFO surface present mode!");
  return VK_PRESENT_MODE_FIFO_KHR;
}


}
