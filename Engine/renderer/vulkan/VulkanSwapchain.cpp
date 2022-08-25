
#include "VulkanSwapchain.h"
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanImages.h"
#include "VulkanWindowSurface.h"
#include <utilities/Logger.h>
#include <window/Window.h>


namespace uncanny
{


void getRequiredSwapchainExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Adding swapchain extension to required ones...");
  pRequiredExtensions->push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


static void destroySwapchain(VkSwapchainKHR* pSwapchain, VkDevice device,
                             const char*pSwapchainLogType);


b32 FRenderContextVulkan::areSwapchainDependenciesCorrect() {
  UTRACE("Validating swapchain dependencies...");

  // make sure that surface caps support used image count...
  u32 minCount{ mVkSurfaceCapabilities.minImageCount };
  u32 maxCount{ mVkSurfaceCapabilities.maxImageCount };
  u32 usedCount{ mSwapchainDependencies.usedImageCount };
  if (not (minCount <= usedCount and usedCount <= maxCount)) {
    UERROR("There is no required image count for swapchain! Cannot create swapchain! Min: {}, "
           "Max: {}, Expected: {}", minCount, maxCount, usedCount);
    return UFALSE;
  }

  // validate image usage
  for (VkImageUsageFlags dependencyImageUsage : mSwapchainDependencies.imageUsageVector) {
    if (not (mVkSurfaceCapabilities.supportedUsageFlags & dependencyImageUsage)) {
      UERROR("{} image usage is not supported!", dependencyImageUsage);
      return UFALSE;
    }
  }

  // validate pre-transform
  if (not (mVkSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)) {
    UERROR("Identity surface pre transform is not supported!");
    return UFALSE;
  }

  UDEBUG("Validated swapchain dependencies, everything is fine!");
  return UTRUE;
}


b32 FRenderContextVulkan::createSwapchain() {
  UTRACE("Creating swapchain...");

  b32 properSwapchainDependencies{ areSwapchainDependenciesCorrect() };
  if (not properSwapchainDependencies) {
    UERROR("Wrong swapchain dependencies passed for creation, cannot acquire images nor present!");
    return UFALSE;
  }

  VkSurfaceFormatKHR imageFormat{ VK_FORMAT_UNDEFINED };
  VkExtent2D imageExtent2D{ mVkSurfaceExtent2D };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };

  b32 detected{ detectSupportedImageFormatByWindowSurface(
      mVkPhysicalDevice, mVkWindowSurface, mSwapchainDependencies.formatCandidatesVector,
      &imageFormat) };
  if (not detected) {
    UERROR("Could not find suitable swapchain presentable image format from window surface!");
    return UFALSE;
  }

  b32 featuresAreSupported{ areFormatsFeaturesDependenciesMetForImageFormat(
      imageFormat.format, imageTiling, mVkPhysicalDevice,
      mSwapchainDependencies.imageFormatFeatureVector, "swapchain") };
  if (not featuresAreSupported) {
    UERROR("Could not create swapchain images, as format features are not supported!");
    return UFALSE;
  }

  // OR every image usage from dependencies
  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : mSwapchainDependencies.imageUsageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = mVkWindowSurface;
  createInfo.minImageCount = mSwapchainDependencies.usedImageCount;
  createInfo.imageFormat = imageFormat.format;
  createInfo.imageColorSpace = imageFormat.colorSpace;
  createInfo.imageExtent = imageExtent2D;
  createInfo.imageArrayLayers = 1; // non-stereoscopic-3D app
  createInfo.imageUsage = imageUsage;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // images are exclusive to queue family
  createInfo.queueFamilyIndexCount = 0;      // for exclusive sharing mode, param is ignored
  createInfo.pQueueFamilyIndices = nullptr; // for exclusive sharing mode, param is ignored
  createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // no transparency with OS
  createInfo.presentMode = mVkPresentMode;
  createInfo.clipped = VK_TRUE; // clipping world that is beyond presented surface (not visible)
  createInfo.oldSwapchain = mVkSwapchainOld;

  VkResult createdSwapchain{ vkCreateSwapchainKHR(mVkDevice, &createInfo, nullptr,
                                                  &mVkSwapchainCurrent) };
  if (createdSwapchain != VK_SUCCESS) {
    UERROR("Could not create swapchain!");
    return UFALSE;
  }

  UTRACE("Retrieving presentable images from swapchain...");
  u32 imageCount{ 0 };
  U_VK_ASSERT( vkGetSwapchainImagesKHR(mVkDevice, mVkSwapchainCurrent, &imageCount, nullptr) );
  std::vector<VkImage> imageVector(imageCount);
  U_VK_ASSERT( vkGetSwapchainImagesKHR(mVkDevice, mVkSwapchainCurrent, &imageCount,
                                       imageVector.data()) );

  // Copying retrieved swapchain images into presentable member handles...
  mImagePresentableVector.resize(imageCount);
  VkExtent3D presentableImageExtent{};
  presentableImageExtent.width = imageExtent2D.width;
  presentableImageExtent.height = imageExtent2D.height;
  presentableImageExtent.depth = 1;
  for(u32 i = 0; i < imageCount; i++) {
    mImagePresentableVector[i].handle = imageVector[i];
    mImagePresentableVector[i].type = EImageType::PRESENTABLE;
    mImagePresentableVector[i].format = imageFormat.format;
    mImagePresentableVector[i].extent = presentableImageExtent;
    mImagePresentableVector[i].tiling = imageTiling;
  }
  imageVector.clear();

  UTRACE("Creating {} image views for swapchain presentable images...", imageCount);
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
    imageViewCreateInfo.image = mImagePresentableVector[i].handle;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = imageFormat.format;
    imageViewCreateInfo.components = componentMapping;
    imageViewCreateInfo.subresourceRange = imageSubresourceRange;

    U_VK_ASSERT( vkCreateImageView(mVkDevice, &imageViewCreateInfo, nullptr,
                                   &mImagePresentableVector[i].handleView) );
  }

  UDEBUG("Created swapchain!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeSwapchain() {
  UTRACE("Closing swapchain...");

  for (FImageVulkan& image : mImagePresentableVector) {
    closeImageVulkan(&image, mVkDevice, "swapchain presentable");
  }
  mImagePresentableVector.clear();

  destroySwapchain(&mVkSwapchainCurrent, mVkDevice, "Current");
  destroySwapchain(&mVkSwapchainOld, mVkDevice, "Old");

  UDEBUG("Closed swapchain!");
  return UTRUE;
}


b32 FRenderContextVulkan::recreateSwapchain() {
  UTRACE("Recreating swapchain...");

  // Firstly destroying images and its image views as they are not needed and
  // those variables will be filled during createSwapchain()
  for (FImageVulkan& image : mImagePresentableVector) {
    closeImageVulkan(&image, mVkDevice, "swapchain presentable");
  }
  mImagePresentableVector.clear();

  // query new information about window surface capabilities
  b32 collectedCapabilities{ collectWindowSurfaceCapabilities() };
  if (not collectedCapabilities) {
    UERROR("Could not update window surface capabilities!");
    return UFALSE;
  }

  // reassigning variables, where old swapchain will be used to creation new one
  mVkSwapchainOld = mVkSwapchainCurrent;
  mVkSwapchainCurrent = VK_NULL_HANDLE;

  // creating new swapchain and its images with image views
  b32 properlyCreatedSwapchain{ createSwapchain() };
  if (not properlyCreatedSwapchain) {
    UERROR("Could not recreate swapchain!");
    return UFALSE;
  }

  // destroying old swapchain
  destroySwapchain(&mVkSwapchainOld, mVkDevice, "OldRecreate");

  UDEBUG("Recreated swapchain!");
  return UTRUE;
}


void destroySwapchain(VkSwapchainKHR* pSwapchain, VkDevice device, const char* pSwapchainLogType) {
  if (*pSwapchain != VK_NULL_HANDLE) {
    UTRACE("Destroying {} swapchain...", pSwapchainLogType);
    vkDestroySwapchainKHR(device, *pSwapchain, nullptr);
    *pSwapchain = VK_NULL_HANDLE;
  }
  else {
    UWARN("{} swapchain is not created, so it won't be destroyed!", pSwapchainLogType);
  }
}


}
