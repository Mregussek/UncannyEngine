
#include "VulkanSwapchain.h"
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include <window/Window.h>


namespace uncanny
{


void getRequiredSwapchainExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Adding swapchain extension to required ones...");
  pRequiredExtensions->push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


template<typename TType>
static TType getProperType(const std::vector<TType>& availableTypes,
                            b32(*isTypeAvailable)(TType type));


static VkExtent2D getProperExtent2D(const VkSurfaceCapabilitiesKHR& capabilities, FWindow* pWindow);


static b32 isSurfaceFormatAvailable(VkSurfaceFormatKHR surfaceFormat);


static b32 isPresentModeAvailable(VkPresentModeKHR presentMode);


b32 FRenderContextVulkan::createSwapchain() {
  UTRACE("Creating swapchain...");

  UDEBUG("Created swapchain!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeSwapchain() {
  UTRACE("Closing swapchain...");

  UDEBUG("Closed swapchain!");
  return UTRUE;
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
  return availableTypes;
}


static b32 isSurfaceFormatAvailable(VkSurfaceFormatKHR surfaceFormat) {
  b32 properFormat{ surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB };
  b32 properColorSpace{ surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  return properFormat && properColorSpace;
}


static b32 isPresentModeAvailable(VkPresentModeKHR presentMode) {
  return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
}


VkExtent2D getProperExtent2D(const VkSurfaceCapabilitiesKHR& capabilities, FWindow* pWindow) {
  UTRACE("Looking for proper extent 2D...");

  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    UTRACE("Returning current extent!");
    return capabilities.currentExtent;
  }

  pWindow->updateFramebufferSize();
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
