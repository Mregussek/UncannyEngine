
#include "../RenderContextVulkan.h"
#include "../VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::recordCommandBuffersGeneral() {
  b32 recordedClearScreen{
      recordCommandBuffersForClearingColorImage(mRenderTargetImageVector, mVkGraphicsCommandPool,
                                                mVkRenderCommandBufferVector) };
  if (not recordedClearScreen) {
    UFATAL("Could not record clear screen command buffers!");
    return UFALSE;
  }

  b32 recordedCopyImage{
      recordCommandBuffersForCopyRenderTargetIntoPresentableImage(mRenderTargetImageVector,
                                                                  mVkImagePresentableVector,
                                                                  mVkGraphicsCommandPool,
                                                                  mVkRenderCommandBufferVector) };
  if (not recordedCopyImage) {
    UFATAL("Could not record copy image command buffers!");
    return UFALSE;
  }

  return UTRUE;
}


b32 FRenderContextVulkan::recordCommandBuffersForClearingColorImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const VkCommandPool& commandPool,
    const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers for clearing color image!");

  b32 properlyResetCommandPoolsAndBuffers{ resetCommandPool(commandPool) };
  if (not properlyResetCommandPoolsAndBuffers) {
    UERROR("Could not reset command pools (with command buffers), so cannot record commands!");
    return UFALSE;
  }

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkClearColorValue clearColor{{ 1.0f, 0.8f, 0.4f, 0.0f }};

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    VkImageMemoryBarrier barrierFromPresentToClear{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrierFromPresentToClear.pNext = nullptr;
    barrierFromPresentToClear.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrierFromPresentToClear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrierFromPresentToClear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierFromPresentToClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierFromPresentToClear.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromPresentToClear.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromPresentToClear.image = renderTargetImages[i].handle;
    barrierFromPresentToClear.subresourceRange = imageSubresourceRange;

    VkImageMemoryBarrier barrierFromClearToPresent{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrierFromClearToPresent.pNext = nullptr;
    barrierFromClearToPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrierFromClearToPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrierFromClearToPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierFromClearToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierFromClearToPresent.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromClearToPresent.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromClearToPresent.image = renderTargetImages[i].handle;
    barrierFromClearToPresent.subresourceRange = imageSubresourceRange;

    VkResult properlyPreparedForCommands{ vkBeginCommandBuffer(commandBuffers[i],
                                                               &commandBufferBeginInfo) };
    if (properlyPreparedForCommands != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    //vkCmdPipelineBarrier(commandBuffers[i],
    //                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    //                     VK_PIPELINE_STAGE_TRANSFER_BIT,
    //                     VkDependencyFlags{ 0 },
    //                     0, nullptr,
    //                     0, nullptr,
    //                     1, &barrierFromPresentToClear);
    // vkCmdClearColorImage -> the least efficient mechanism on tile-based GPU architectures
    vkCmdClearColorImage(commandBuffers[i],
                         renderTargetImages[i].handle,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         &clearColor,
                         1, &imageSubresourceRange);
    //vkCmdPipelineBarrier(commandBuffers[i],
    //                     VK_PIPELINE_STAGE_TRANSFER_BIT,
    //                     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    //                     VkDependencyFlags{ 0 },
    //                     0, nullptr,
    //                     0, nullptr,
    //                     1, &barrierFromClearToPresent);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(commandBuffers[i]) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Properly recorded command buffers for clearing color image!");
  return UTRUE;
}


b32 FRenderContextVulkan::recordCommandBuffersForCopyRenderTargetIntoPresentableImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<VkImage>& presentableImages,
    const VkCommandPool& commandPool,
    const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers for copying render target images into presentable ones!");

  b32 properlyResetCommandPoolsAndBuffers{ resetCommandPool(commandPool) };
  if (not properlyResetCommandPoolsAndBuffers) {
    UERROR("Could not reset command pools (with command buffers), so cannot record commands!");
    return UFALSE;
  }

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    VkResult properlyPreparedForCommands{ vkBeginCommandBuffer(commandBuffers[i],
                                                               &commandBufferBeginInfo) };
    if (properlyPreparedForCommands != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    VkImageSubresourceLayers subresourceLayers{};
    subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceLayers.mipLevel = 1;
    subresourceLayers.baseArrayLayer = 0;
    subresourceLayers.layerCount = 1;

    VkOffset3D offset{};
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;

    VkImageCopy imageCopy{};
    imageCopy.srcSubresource = subresourceLayers;
    imageCopy.srcOffset = offset;
    imageCopy.dstSubresource = subresourceLayers;
    imageCopy.dstOffset = offset;
    imageCopy.extent = renderTargetImages[i].extent;

    vkCmdCopyImage(commandBuffers[i], renderTargetImages[i].handle, VK_IMAGE_LAYOUT_UNDEFINED,
                   presentableImages[i], VK_IMAGE_LAYOUT_GENERAL,
                   1, &imageCopy);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(commandBuffers[i]) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Properly recorded command buffers for copying render targets into presentable images!");
  return UTRUE;
}


}
