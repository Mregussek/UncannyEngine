
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny
{


template<typename TType>
static TType getProperType(const std::vector<TType>& availableTypes,
                           b32(*isTypeAvailable)(TType type));


static VkExtent2D getProperExtent2D(const VkSurfaceCapabilitiesKHR& capabilities,
                                    const FWindow* pWindow);


static b32 isSuitableSurfaceFormatAvailable(VkSurfaceFormatKHR surfaceFormat);


static b32 isSuitablePresentModeAvailable(VkPresentModeKHR presentMode);


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
    U_VK_ASSERT( vkCreateWin32SurfaceKHR(mVkInstance, &createInfo, nullptr, &mVkWindowSurface) );
  }

  // when window surface is created we can query information about it...
  // query surface format info...
  u32 formatCount{ 0 };
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                    &formatCount, nullptr) );

  std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                    &formatCount, availableFormats.data()) );

  mVkSurfaceFormat = getProperType(availableFormats, isSuitableSurfaceFormatAvailable);

  // query present modes info...
  u32 presentCount{ 0 };
  U_VK_ASSERT( vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &presentCount, nullptr) );

  std::vector<VkPresentModeKHR> availablePresentModes(presentCount);
  U_VK_ASSERT( vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &presentCount, availablePresentModes.data()) );

  mVkPresentMode = getProperType(availablePresentModes, isSuitablePresentModeAvailable);

  // query surface capabilities info...
  U_VK_ASSERT( vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, mVkWindowSurface,
                                                         &mVkSurfaceCapabilities) );

  // query image extent info...
  mVkImageExtent2D = getProperExtent2D(mVkSurfaceCapabilities, mSpecs.pWindow);

  UDEBUG("Created window surface!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeWindowSurface() {
  UTRACE("Closing Window Surface...");

  if (mVkWindowSurface == VK_NULL_HANDLE) {
    UWARN("Window surface is not created, so it won't be closed!");
    return UTRUE;
  }

  vkDestroySurfaceKHR(mVkInstance, mVkWindowSurface, nullptr);

  UDEBUG("Closed Window Surface!");
  return UTRUE;
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


template<typename TType>
static TType getProperType(const std::vector<TType>& availableTypes,
                           b32(*isTypeAvailable)(TType type)) {
  UTRACE("Looking for proper {}...", typeid(TType).name());

  if (availableTypes.empty()) {
    UWARN("Returning none as {} vector is empty!", typeid(TType).name());
    return {};
  }

  auto it = std::find_if(availableTypes.cbegin(), availableTypes.cend(), isTypeAvailable);
  if (it != availableTypes.cend()) {
    UTRACE("Returning proper {} which is available!", typeid(TType).name());
    return *it;
  }

  UWARN("Returning fallback {}, as expected one is not available!", typeid(TType).name());
  return availableTypes[0];
}


b32 isSuitableSurfaceFormatAvailable(VkSurfaceFormatKHR surfaceFormat) {
  b32 properFormat{ surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB };
  b32 properColorSpace{ surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  return properFormat && properColorSpace;
}


b32 isSuitablePresentModeAvailable(VkPresentModeKHR presentMode) {
  return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
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


}
