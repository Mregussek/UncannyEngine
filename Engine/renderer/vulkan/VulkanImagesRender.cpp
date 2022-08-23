
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include "VulkanImages.h"


namespace uncanny
{


static b32 createRenderTargetImage(VkPhysicalDevice physicalDevice,
                                   VkDevice device,
                                   VkExtent2D surfaceExtent,
                                   VkFormat surfaceFormat,
                                   FImageVulkan* pOutRenderTargetImage);


b32 FRenderContextVulkan::createRenderTargetImages() {
  u32 imageCount{ static_cast<u32>(mImagePresentableVector.size()) };
  UTRACE("Creating {} Render Target Images...", imageCount);

  mImageRenderTargetVector.resize(imageCount);

  for (u32 i = 0; i < imageCount; i++) {
    UTRACE("Creating render target image {}...", i);
    b32 createdProperly{ createRenderTargetImage(mVkPhysicalDevice, mVkDevice,
                                                 mVkImageExtent2D, mVkSurfaceFormat.format,
                                                 &mImageRenderTargetVector[i]) };
    if (not createdProperly) {
      UERROR("Could not create render target image at index {}", i);
      return UFALSE;
    }
    UDEBUG("Successfully created render target image!");
  }

  UDEBUG("Created Render Target Images!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeRenderTargetImages() {
  UTRACE("Closing Render Target Images...");

  if (mImageRenderTargetVector.empty()) {
    UWARN("As render target image vector is not initialized (it is empty), not closing anything!");
    return UTRUE;
  }

  for (FImageVulkan& renderTargetImage : mImageRenderTargetVector) {
    b32 closedProperly{ closeImageVulkan(&renderTargetImage, mVkDevice, "render target") };
    if (not closedProperly) {
      UERROR("Could not close properly render target image!");
      return UFALSE;
    }
  }
  mImageRenderTargetVector.clear();

  UDEBUG("Closed Render Target Images!");
  return UTRUE;
}


b32 FRenderContextVulkan::recreateRenderTargetImages() {
  UTRACE("Recreating render target images...");

  b32 closed{ closeRenderTargetImages() };
  if (not closed) {
    UERROR("Could not close render target images, some issue appeared!");
    return UFALSE;
  }

  b32 created{ createRenderTargetImages() };
  if (not created) {
    UERROR("Could not create render target images during recreation!");
    return UFALSE;
  }

  UDEBUG("Recreated render target images!");
  return UTRUE;
}


b32 createRenderTargetImage(VkPhysicalDevice physicalDevice,
                            VkDevice device,
                            VkExtent2D surfaceExtent,
                            VkFormat surfaceFormat,
                            FImageVulkan* pOutRenderTargetImage) {
  pOutRenderTargetImage->format = surfaceFormat;
  pOutRenderTargetImage->type = EImageType::RENDER_TARGET;

  pOutRenderTargetImage->extent.width = surfaceExtent.width;
  pOutRenderTargetImage->extent.height = surfaceExtent.height;
  pOutRenderTargetImage->extent.depth = 1;

  VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = 0;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = pOutRenderTargetImage->format;
  imageCreateInfo.extent = pOutRenderTargetImage->extent;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
      VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.queueFamilyIndexCount = 0;
  imageCreateInfo.pQueueFamilyIndices = nullptr;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VkResult createdImage{ vkCreateImage(device, &imageCreateInfo, nullptr,
                                       &pOutRenderTargetImage->handle) };
  if (createdImage != VK_SUCCESS) {
    UERROR("Could not create render target image!");
    return UFALSE;
  }

  VkMemoryRequirements memoryReqs{};
  vkGetImageMemoryRequirements(device, pOutRenderTargetImage->handle, &memoryReqs);

  VkPhysicalDeviceMemoryProperties memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

  u32 memoryTypeIndex{ findMemoryIndex(memoryProperties, memoryReqs.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) };

  if (memoryTypeIndex == UUNUSED) {
    UERROR("Required memory type index not found, render target image is not valid!");
    return UFALSE;
  }

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.pNext = nullptr;
  memoryAllocateInfo.allocationSize = memoryReqs.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

  VkResult allocated{ vkAllocateMemory(device, &memoryAllocateInfo, nullptr,
                                       &pOutRenderTargetImage->deviceMemory) };
  if (allocated != VK_SUCCESS) {
    UERROR("Could not allocate device memory for render target image!");
    return UFALSE;
  }

  VkDeviceSize memoryOffset{ 0 };
  VkResult bound{ vkBindImageMemory(device, pOutRenderTargetImage->handle,
                                    pOutRenderTargetImage->deviceMemory, memoryOffset) };
  if (bound != VK_SUCCESS) {
    UERROR("Could not bind device memory for render target image!");
    return UFALSE;
  }

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
  imageViewCreateInfo.image = pOutRenderTargetImage->handle;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = pOutRenderTargetImage->format;
  imageViewCreateInfo.components = componentMapping;
  imageViewCreateInfo.subresourceRange = imageSubresourceRange;

  VkResult createdView{ vkCreateImageView(device, &imageViewCreateInfo, nullptr,
                                          &pOutRenderTargetImage->handleView) };
  if (createdView != VK_SUCCESS) {
    UERROR("Could not create render target image view!");
    return UFALSE;
  }
  return UTRUE;
}


}
