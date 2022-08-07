
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


b32 FRenderContextVulkan::createSwapchain() {
  UTRACE("Creating swapchain...");

  // TODO: if some queues using the swapchain will be from other queue families,
  // there is need to define concurrent sharing mode and pass info about those families

  VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = mVkWindowSurface;
  createInfo.minImageCount = std::max(2u, mVkSurfaceCapabilities.minImageCount);
  createInfo.imageFormat = mVkSurfaceFormat.format;
  createInfo.imageColorSpace = mVkSurfaceFormat.colorSpace;
  createInfo.imageExtent = mVkImageExtent2D;
  createInfo.imageArrayLayers = 1; // non-stereoscopic-3D app
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // color images
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // images are exclusive to queue family
  createInfo.queueFamilyIndexCount = 0;      // for exclusive sharing mode, param is ignored
  createInfo.pQueueFamilyIndices = nullptr; // for exclusive sharing mode, param is ignored
  createInfo.preTransform = mVkSurfaceCapabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // no transparency with OS
  createInfo.presentMode = mVkPresentMode;
  createInfo.clipped = VK_TRUE; // clipping world that is beyond presented surface (not visible)
  createInfo.oldSwapchain = mVkSwapchainOld;

  U_VK_ASSERT( vkCreateSwapchainKHR(mVkDevice, &createInfo, nullptr, &mVkSwapchainCurrent) );

  UDEBUG("Created swapchain!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeSwapchain() {
  UTRACE("Closing swapchain...");

  if (mVkSwapchainCurrent != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(mVkDevice, mVkSwapchainCurrent, nullptr);
  }
  else {
    UWARN("Current swapchain is not created, so it won't be destroyed!");
  }
  if (mVkSwapchainOld != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(mVkDevice, mVkSwapchainOld, nullptr);
  }
  else {
    UWARN("Old swapchain is not created, so it won't be destroyed!");
  }

  UDEBUG("Closed swapchain!");
  return UTRUE;
}


}
