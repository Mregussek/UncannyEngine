
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <renderer/vulkan_context/VulkanWindowSurface.h>
#include <utilities/Logger.h>
#include "VulkanImages.h"


namespace uncanny
{


b32 FRendererVulkan::createDepthImage() {
  UTRACE("Creating depth image...");

  // TODO: Make sure no other format be used
  UTRACE("Using 0-indexed depth format from dependencies!");
  VkFormat imageFormat{ mImageDependencies.depth.formatCandidatesVector[0].format };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };
  VkExtent2D surfaceExtent{ mContextPtr->SurfaceExtent() };
  VkExtent3D imageExtent{ surfaceExtent.width, surfaceExtent.height, 1 };

  b32 featuresAreSupported{ areFormatsFeaturesDependenciesMetForImageFormat(
      imageFormat, imageTiling, mContextPtr->PhysicalDevice(),
      mImageDependencies.depth.formatsFeatureVector, "depth") };
  if (not featuresAreSupported) {
    UERROR("Could not create depth images, as format features are not supported!");
    return UFALSE;
  }

  // create depth image...
  mVkDepthFormat = imageFormat;
  mDepthImage.format = mVkDepthFormat;
  mDepthImage.tiling = imageTiling;
  mDepthImage.type = EImageType::DEPTH;
  mDepthImage.extent = imageExtent;

  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : mImageDependencies.depth.usageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = 0;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = mDepthImage.format;
  imageCreateInfo.extent = mDepthImage.extent;
  imageCreateInfo.mipLevels = 4;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = mDepthImage.tiling;
  imageCreateInfo.usage = imageUsage;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.queueFamilyIndexCount = 0;
  imageCreateInfo.pQueueFamilyIndices = nullptr;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  U_VK_ASSERT( vkCreateImage(mContextPtr->Device(), &imageCreateInfo, nullptr,
                             &mDepthImage.handle) );

  // allocate memory for depth image
  b32 allocated{ allocateAndBindImageMemory(mContextPtr->PhysicalDevice(), mContextPtr->Device(),
                                            &mDepthImage, "depth") };
  if (not allocated) {
    UERROR("Could not allocate render target image memory!");
    return UFALSE;
  }

  // create depth image view...
  VkComponentMapping componentMapping{};
  componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.image = mDepthImage.handle;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = mDepthImage.format;
  imageViewCreateInfo.components = componentMapping;
  imageViewCreateInfo.subresourceRange = imageSubresourceRange;

  U_VK_ASSERT( vkCreateImageView(mContextPtr->Device(), &imageViewCreateInfo,nullptr,
                                 &mDepthImage.handleView) );

  UDEBUG("Created depth image!");
  return UTRUE;
}


b32 FRendererVulkan::closeDepthImage() {
  UTRACE("Closing depth image...");

  b32 closedProperly{ closeImageVulkan(&mDepthImage, mContextPtr->Device(), "depth") };
  if (not closedProperly) {
    UERROR("Could not close properly depth image!");
    return UFALSE;
  }
  mDepthImage = {};

  UDEBUG("Closed depth image!");
  return UTRUE;
}


b32 FRendererVulkan::recreateDepthImage() {
  UTRACE("Recreating depth image...");

  b32 closed{ closeDepthImage() };
  if (not closed) {
    UERROR("Could not close depth image, some issue appeared!");
    return UFALSE;
  }

  b32 created{ createDepthImage() };
  if (not created) {
    UERROR("Could not create depth image during recreation!");
    return UFALSE;
  }

  UDEBUG("Recreated depth image!");
  return UTRUE;
}


}
