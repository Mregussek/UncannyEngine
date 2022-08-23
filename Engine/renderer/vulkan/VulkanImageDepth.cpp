
#include "RenderContextVulkan.h"
#include "VulkanImageDepth.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include "VulkanImages.h"


namespace uncanny
{


b32 FRenderContextVulkan::createDepthImage() {
  UTRACE("Creating depth image...");

  if (mVkDepthFormat == VK_FORMAT_UNDEFINED) {
    UTRACE("Depth format is not detected! Searching...");
    b32 detectedDepthFormat{
      detectSupportedDepthFormat(mVkPhysicalDevice,
                                 mPhysicalDeviceDependencies.depthFormatDependencies,
                                 &mVkDepthFormat) };
    if (not detectedDepthFormat) {
      UERROR("Could not detect depth format!");
      return UFALSE;
    }

    UTRACE("Found depth format, now can create depth image...");
  }

  mDepthImage.format = mVkDepthFormat;
  mDepthImage.type = EImageType::DEPTH;

  // create depth image...
  VkExtent3D imageExtent{};
  imageExtent.width = mVkSurfaceExtent2D.width;
  imageExtent.height = mVkSurfaceExtent2D.height;
  imageExtent.depth = 1;

  VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = 0;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = mDepthImage.format;
  imageCreateInfo.extent = imageExtent;
  imageCreateInfo.mipLevels = 4;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
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
  componentMapping.r = VK_COMPONENT_SWIZZLE_R;
  componentMapping.g = VK_COMPONENT_SWIZZLE_G;
  componentMapping.b = VK_COMPONENT_SWIZZLE_B;
  componentMapping.a = VK_COMPONENT_SWIZZLE_A;

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


b32 detectSupportedDepthFormat(VkPhysicalDevice physicalDevice,
                               const std::vector<VkFormat>& depthFormatCandidates,
                               VkFormat* pOutDepthFormat) {
  VkFormatFeatureFlagBits flags{ VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT };
  for (u32 i = 0; i < depthFormatCandidates.size(); i++) {
    VkFormatProperties formatProperties{};
    vkGetPhysicalDeviceFormatProperties(physicalDevice, depthFormatCandidates[i],
                                        &formatProperties);

    if ((formatProperties.linearTilingFeatures & flags) == flags) {
      *pOutDepthFormat = depthFormatCandidates[i];
      UTRACE("Detected depth format {} at index {}!", *pOutDepthFormat, i);
      return UTRUE;
    }
    else if ((formatProperties.optimalTilingFeatures & flags) == flags) {
      *pOutDepthFormat = depthFormatCandidates[i];
      UTRACE("Detected depth format {} at index {}!", *pOutDepthFormat, i);
      return UTRUE;
    }
  }

  UERROR("Could not detect depth format from candidates!");
  return UFALSE;
}


}
