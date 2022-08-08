
#include "RenderContextVulkan.h"
#include "VulkanImageDepth.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


static u32 findMemoryIndex(VkPhysicalDeviceMemoryProperties memoryProperties, u32 typeFilter,
                           VkMemoryPropertyFlagBits flags);


b32 FRenderContextVulkan::createDepthImages() {
  UTRACE("Creating depth images...");

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

    UTRACE("Found depth format, now can create depth images...");
  }

  // create depth image...
  VkExtent3D depthImageExtent{};
  depthImageExtent.width = mVkImageExtent2D.width;
  depthImageExtent.height = mVkImageExtent2D.height;
  depthImageExtent.depth = 1;

  VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = 0;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = mVkDepthFormat;
  imageCreateInfo.extent = depthImageExtent;
  imageCreateInfo.mipLevels = 4;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.queueFamilyIndexCount = 0;
  imageCreateInfo.pQueueFamilyIndices = nullptr;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  U_VK_ASSERT( vkCreateImage(mVkDevice, &imageCreateInfo, nullptr, &mVkDepthImage) );

  // allocate memory for depth image
  VkMemoryRequirements memoryReqs;
  vkGetImageMemoryRequirements(mVkDevice, mVkDepthImage, &memoryReqs);

  u32 memoryTypeIndex{ findMemoryIndex(mVkPhysicalDeviceMemoryProperties,
                                       memoryReqs.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) };
  if (memoryTypeIndex == UUNUSED) {
    UERROR("Required memory type index not found, depth image is not valid!");
    return UFALSE;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = memoryReqs.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  U_VK_ASSERT( vkAllocateMemory(mVkDevice, &memoryAllocateInfo, nullptr, &mVkDepthImageMemory) );

  VkDeviceSize memoryOffset{ 0 };
  U_VK_ASSERT( vkBindImageMemory(mVkDevice, mVkDepthImage, mVkDepthImageMemory, memoryOffset) );

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
  imageViewCreateInfo.image = mVkDepthImage;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = mVkDepthFormat;
  imageViewCreateInfo.components = componentMapping;
  imageViewCreateInfo.subresourceRange = imageSubresourceRange;

  U_VK_ASSERT( vkCreateImageView(mVkDevice, &imageViewCreateInfo,nullptr, &mVkDepthImageView) );

  UDEBUG("Created depth images!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeDepthImages() {
  UTRACE("Closing depth images...");

  if (mVkDepthFormat != VK_FORMAT_UNDEFINED) {
    UTRACE("Clearing depth format variable...");
    mVkDepthFormat = VK_FORMAT_UNDEFINED;
  }

  if (mVkDepthImageView != VK_NULL_HANDLE) {
    UTRACE("Destroying depth image view...");
    vkDestroyImageView(mVkDevice, mVkDepthImageView, nullptr);
  }
  else {
    UWARN("As depth image view is not created, it is not destroyed!");
  }

  if (mVkDepthImageMemory != VK_NULL_HANDLE) {
    UTRACE("Freeing depth image memory...");
    vkFreeMemory(mVkDevice, mVkDepthImageMemory, nullptr);
  }
  else {
    UWARN("As depth image memory is not allocated, it won't be freed!");
  }

  if (mVkDepthImage != VK_NULL_HANDLE) {
    UTRACE("Destroying depth image...");
    vkDestroyImage(mVkDevice, mVkDepthImage, nullptr);
  }
  else {
    UWARN("As depth image is not created, it is not destroyed!");
  }

  UDEBUG("Closed depth images!");
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


u32 findMemoryIndex(VkPhysicalDeviceMemoryProperties memoryProperties, u32 typeFilter,
                    VkMemoryPropertyFlagBits flags) {
  for (u32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
    // Check each memory type to see if its bit is set to 1.
    if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags) {
      return i;
    }
  }

  UWARN("Unable to find suitable memory type!");
  return UUNUSED;
}


}
