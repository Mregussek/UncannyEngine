
#include "VulkanSwapchain.h"
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include <window/Window.h>


namespace uncanny
{


void getRequiredSwapchainExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Adding swapchain extension to required ones...");
  pRequiredExtensions->push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


static VkCompositeAlphaFlagBitsKHR getSupportedCompositeAlpha(
    const VkSurfaceCapabilitiesKHR& surfaceCaps) {
  if (surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
    return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  }

  return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
}


b32 FRenderContextVulkan::createSwapchain() {
  UTRACE("Creating swapchain...");

  VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = mVkWindowSurface;
  createInfo.minImageCount = std::max(2u, mVkSurfaceCapabilities.minImageCount);
  createInfo.imageFormat = mVkSurfaceFormat.format;
  createInfo.imageColorSpace = mVkSurfaceFormat.colorSpace;
  createInfo.imageExtent = mVkImageExtent2D;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 1;
  createInfo.pQueueFamilyIndices = &mQueueFamilyVector[mGraphicsQueueFamilyIndex].index;
  createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  createInfo.compositeAlpha = getSupportedCompositeAlpha(mVkSurfaceCapabilities);
  createInfo.presentMode = mVkPresentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = mVkSwapchainOld;

  UDEBUG("Created swapchain!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeSwapchain() {
  UTRACE("Closing swapchain...");

  UDEBUG("Closed swapchain!");
  return UTRUE;
}


}
