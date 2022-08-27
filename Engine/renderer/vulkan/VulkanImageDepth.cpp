
#include "RenderContextVulkan.h"
#include "VulkanWindowSurface.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include "VulkanImages.h"


namespace uncanny
{


b32 FRenderContextVulkan::createDepthImage() {
  UTRACE("Creating depth image...");

  // TODO: Make sure no other format be used
  UTRACE("Using 0-indexed depth format from dependencies!");
  VkFormat imageFormat{ mImageDependencies.depth.formatCandidatesVector[0].format };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };
  VkExtent3D imageExtent{ mVkSurfaceExtent2D.width, mVkSurfaceExtent2D.height, 1 };

  b32 featuresAreSupported{ areFormatsFeaturesDependenciesMetForImageFormat(
      imageFormat, imageTiling, mVkPhysicalDevice,
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

  U_VK_ASSERT( vkCreateImage(mVkDevice, &imageCreateInfo, nullptr, &mDepthImage.handle) );

  // allocate memory for depth image
  VkMemoryRequirements memoryReqs{};
  vkGetImageMemoryRequirements(mVkDevice, mDepthImage.handle, &memoryReqs);

  VkPhysicalDeviceMemoryProperties memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &memoryProperties);

  u32 memoryTypeIndex{ findMemoryIndex(memoryProperties, memoryReqs.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) };
  if (memoryTypeIndex == UUNUSED) {
    UERROR("Required memory type index not found, depth image is not valid!");
    return UFALSE;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = memoryReqs.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  U_VK_ASSERT( vkAllocateMemory(mVkDevice, &memoryAllocateInfo, nullptr,
                                &mDepthImage.deviceMemory) );

  VkDeviceSize memoryOffset{ 0 };
  U_VK_ASSERT( vkBindImageMemory(mVkDevice, mDepthImage.handle, mDepthImage.deviceMemory,
                                 memoryOffset) );

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

  U_VK_ASSERT( vkCreateImageView(mVkDevice, &imageViewCreateInfo,nullptr, &mDepthImage.handleView) );

  UDEBUG("Created depth image!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeDepthImage() {
  UTRACE("Closing depth image...");

  b32 closedProperly{ closeImageVulkan(&mDepthImage, mVkDevice, "depth") };
  if (not closedProperly) {
    UERROR("Could not close properly depth image!");
    return UFALSE;
  }
  mDepthImage = {};

  UDEBUG("Closed depth image!");
  return UTRUE;
}


b32 FRenderContextVulkan::recreateDepthImage() {
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
