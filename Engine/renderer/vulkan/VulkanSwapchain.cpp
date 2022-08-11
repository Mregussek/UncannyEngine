
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

  // make sure that surface caps support used image count...
  if (mSwapchainDependencies.usedImageCount > mVkSurfaceCapabilities.minImageCount) {
    UERROR("There is required more image count for swapchain that capabilities offer! Cannot "
           "create swapchain!");
    return UFALSE;
  }

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

  // Create presentable Images and its ImageViews...
  u32 imageCount{ 0 };
  U_VK_ASSERT( vkGetSwapchainImagesKHR(mVkDevice, mVkSwapchainCurrent, &imageCount, nullptr) );

  mVkImagePresentableVector.resize(imageCount);
  mVkImagePresentableViewVector.resize(imageCount);
  U_VK_ASSERT( vkGetSwapchainImagesKHR(mVkDevice, mVkSwapchainCurrent, &imageCount,
                                       mVkImagePresentableVector.data()) );

  for (u32 i = 0; i < imageCount; ++i) {
    VkComponentMapping componentMapping{};
    componentMapping.r = VK_COMPONENT_SWIZZLE_R;
    componentMapping.g = VK_COMPONENT_SWIZZLE_G;
    componentMapping.b = VK_COMPONENT_SWIZZLE_B;
    componentMapping.a = VK_COMPONENT_SWIZZLE_A;

    VkImageSubresourceRange imageSubresourceRange{};
    imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubresourceRange.baseMipLevel = 0;
    imageSubresourceRange.levelCount = 1;
    imageSubresourceRange.baseArrayLayer = 0;
    imageSubresourceRange.layerCount = 1;

    VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    imageViewCreateInfo.pNext = nullptr;
    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = mVkImagePresentableVector[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = mVkSurfaceFormat.format;
    imageViewCreateInfo.components = componentMapping;
    imageViewCreateInfo.subresourceRange = imageSubresourceRange;

    U_VK_ASSERT( vkCreateImageView(mVkDevice, &imageViewCreateInfo, nullptr,
                                   &mVkImagePresentableViewVector[i]) );
  }

  UDEBUG("Created swapchain!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeSwapchain() {
  UTRACE("Closing swapchain...");

  for (u32 i = 0; i < mVkImagePresentableVector.size(); i++) {
    UTRACE("Destroying image {}...", i);
    if (mVkImagePresentableVector[i] != VK_NULL_HANDLE) {
      // these images are destroyed during vkDestroySwapchainKHR
      mVkImagePresentableVector[i] = VK_NULL_HANDLE;
      continue;
    }
    UWARN("As image {} is not created, so it is not destroyed!", i);
  }
  mVkImagePresentableVector.clear();

  for (u32 i = 0; i < mVkImagePresentableViewVector.size(); i++) {
    UTRACE("Destroying image view {}...", i);
    if (mVkImagePresentableViewVector[i] != VK_NULL_HANDLE) {
      vkDestroyImageView(mVkDevice, mVkImagePresentableViewVector[i], nullptr);
      mVkImagePresentableViewVector[i] = VK_NULL_HANDLE;
      continue;
    }
    UWARN("As image view {} is not created, so it is not destroyed!", i);
  }
  mVkImagePresentableViewVector.clear();

  if (mVkSwapchainCurrent != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(mVkDevice, mVkSwapchainCurrent, nullptr);
    mVkSwapchainCurrent = VK_NULL_HANDLE;
  }
  else {
    UWARN("Current swapchain is not created, so it won't be destroyed!");
  }
  if (mVkSwapchainOld != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(mVkDevice, mVkSwapchainOld, nullptr);
    mVkSwapchainOld = VK_NULL_HANDLE;
  }
  else {
    UWARN("Old swapchain is not created, so it won't be destroyed!");
  }

  UDEBUG("Closed swapchain!");
  return UTRUE;
}


}
