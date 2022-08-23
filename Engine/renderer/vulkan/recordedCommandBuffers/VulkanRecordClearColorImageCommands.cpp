
#include "../RenderContextVulkan.h"
#include "../VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::recordCommandBuffersGeneral() {
  b32 properlyResetCommandPoolsAndBuffers{ resetCommandPool(mVkGraphicsCommandPool) };
  if (not properlyResetCommandPoolsAndBuffers) {
    UERROR("Could not reset command pools (with command buffers), so cannot record commands!");
    return UFALSE;
  }

  b32 recordedClearScreen{ recordClearColorImage(mImageRenderTargetVector,
                                                 mVkRenderCommandBufferVector) };
  if (not recordedClearScreen) {
    UFATAL("Could not record clear screen command buffers!");
    return UFALSE;
  }

  b32 recordedCopyImage{ recordCopyRenderTargetIntoPresentableImage(mImageRenderTargetVector,
                                                                    mImagePresentableVector,
                                                                    mVkCopyCommandBufferVector) };
  if (not recordedCopyImage) {
    UFATAL("Could not record copy image command buffers!");
    return UFALSE;
  }

  return UTRUE;
}


b32 FRenderContextVulkan::recordClearColorImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<VkCommandBuffer>& commandBuffers) const {
  UTRACE("Recording command buffers for clearing color image!");

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

  VkImageMemoryBarrier barrierUnknownToClear{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
  barrierUnknownToClear.pNext = nullptr;
  barrierUnknownToClear.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierUnknownToClear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierUnknownToClear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrierUnknownToClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierUnknownToClear.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
  barrierUnknownToClear.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
  barrierUnknownToClear.image = VK_NULL_HANDLE; // will be filled later
  barrierUnknownToClear.subresourceRange = imageSubresourceRange;

  VkImageMemoryBarrier barrierClearToGeneral{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
  barrierClearToGeneral.pNext = nullptr;
  barrierClearToGeneral.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierClearToGeneral.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierClearToGeneral.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierClearToGeneral.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
  barrierClearToGeneral.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
  barrierClearToGeneral.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
  barrierClearToGeneral.image = VK_NULL_HANDLE; // will be filled later
  barrierClearToGeneral.subresourceRange = imageSubresourceRange;

  VkClearColorValue clearColor{{ 1.0f, 0.8f, 0.4f, 0.0f }};

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    barrierUnknownToClear.image = renderTargetImages[i].handle;
    barrierClearToGeneral.image = renderTargetImages[i].handle;

    VkResult properlyPreparedForCommands{ vkBeginCommandBuffer(commandBuffers[i],
                                                               &commandBufferBeginInfo) };
    if (properlyPreparedForCommands != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         1, &barrierUnknownToClear);
    // vkCmdClearColorImage -> the least efficient mechanism on tile-based GPU architectures
    vkCmdClearColorImage(commandBuffers[i],
                         renderTargetImages[i].handle,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         &clearColor,
                         1, &imageSubresourceRange);
    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         1, &barrierClearToGeneral);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(commandBuffers[i]) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Properly recorded command buffers for clearing color image!");
  return UTRUE;
}


b32 FRenderContextVulkan::recordCopyRenderTargetIntoPresentableImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<FImageVulkan>& presentableImages,
    const std::vector<VkCommandBuffer>& commandBuffers) const {
  UTRACE("Recording command buffers for copying render target images into presentable ones!");

  if constexpr (U_VK_DEBUG) {
    UTRACE("Making sure that command buffers can be recorded!");
    if (renderTargetImages.size() == presentableImages.size() and renderTargetImages.empty()) {
      UERROR("No render target images and no presentable images! Cannot record cmd!");
      return UFALSE;
    }

    for (u32 i = 0; i < renderTargetImages.size(); i++) {
      if (renderTargetImages[i].type != EImageType::RENDER_TARGET) {
        UERROR("Render target image has unsupported type: {}", (i32)renderTargetImages[i].type);
        return UFALSE;
      }
      if (presentableImages[i].type != EImageType::PRESENTABLE) {
        UERROR("Render target image has unsupported type: {}", (i32)renderTargetImages[i].type);
        return UFALSE;
      }
      if (renderTargetImages[i].format != presentableImages[i].format) {
        UERROR("Render target image and presentable image have different formats! rt {} pr {}",
               renderTargetImages[i].format, presentableImages[i].format);
        return UFALSE;
      }
      if (renderTargetImages[i].extent.height != presentableImages[i].extent.height) {
        UERROR("Render target image height does not equal to presentable one's height! rt {} pr {}",
               renderTargetImages[i].extent.height, presentableImages[i].extent.height);
        return UFALSE;
      }
      if (renderTargetImages[i].extent.width != presentableImages[i].extent.width) {
        UERROR("Render target image width does not equal to presentable one's width! rt {} pr {}",
               renderTargetImages[i].extent.width, presentableImages[i].extent.width);
        return UFALSE;
      }
    }
  }

  VkImageSubresourceLayers subresourceLayers{};
  subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceLayers.mipLevel = 0;
  subresourceLayers.baseArrayLayer = 0;
  subresourceLayers.layerCount = 1;

  VkImageCopy copyRegion{};
  copyRegion.srcSubresource = subresourceLayers;
  copyRegion.srcOffset = { 0, 0, 0 };
  copyRegion.dstSubresource = subresourceLayers;
  copyRegion.dstOffset = { 0, 0, 0 };
  copyRegion.extent = renderTargetImages[0].extent;

  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkImageMemoryBarrier defaultBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
  defaultBarrier.pNext = nullptr;
  defaultBarrier.srcAccessMask = VK_ACCESS_NONE; // will be filled later
  defaultBarrier.dstAccessMask = VK_ACCESS_NONE; // will be filled later
  defaultBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // will be filled later
  defaultBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED; // will be filled later
  defaultBarrier.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
  defaultBarrier.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
  defaultBarrier.image = VK_NULL_HANDLE; // will fill later
  defaultBarrier.subresourceRange = imageSubresourceRange;

  VkImageMemoryBarrier barrierUndefinedToReadRenderTarget{ defaultBarrier };
  barrierUndefinedToReadRenderTarget.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierUndefinedToReadRenderTarget.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierUndefinedToReadRenderTarget.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrierUndefinedToReadRenderTarget.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrierUndefinedToReadRenderTarget.image = VK_NULL_HANDLE; // will fill later

  VkImageMemoryBarrier barrierReadToGeneralRenderTarget{ defaultBarrier };
  barrierReadToGeneralRenderTarget.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierReadToGeneralRenderTarget.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierReadToGeneralRenderTarget.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrierReadToGeneralRenderTarget.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
  barrierReadToGeneralRenderTarget.image = VK_NULL_HANDLE; // will fill later

  VkImageMemoryBarrier barrierUndefinedToTransferPresentable{ defaultBarrier };
  barrierUndefinedToTransferPresentable.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierUndefinedToTransferPresentable.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierUndefinedToTransferPresentable.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrierUndefinedToTransferPresentable.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierUndefinedToTransferPresentable.image = VK_NULL_HANDLE; // will fill later

  VkImageMemoryBarrier barrierTransferToPresentPresentable{ defaultBarrier };
  barrierTransferToPresentPresentable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierTransferToPresentPresentable.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierTransferToPresentPresentable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierTransferToPresentPresentable.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrierTransferToPresentPresentable.image = VK_NULL_HANDLE; // will fill later

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    barrierUndefinedToTransferPresentable.image = presentableImages[i].handle;
    barrierTransferToPresentPresentable.image = presentableImages[i].handle;

    barrierUndefinedToReadRenderTarget.image = renderTargetImages[i].handle;
    barrierReadToGeneralRenderTarget.image = renderTargetImages[i].handle;

    constexpr u32 imageMemoryBarrierCount{ 2 };
    VkImageMemoryBarrier imageMemoryBarrierUndefinedToOperateArray[imageMemoryBarrierCount]{
      barrierUndefinedToTransferPresentable, barrierUndefinedToReadRenderTarget };
    VkImageMemoryBarrier imageMemoryBarrierOperateToPresentArray[imageMemoryBarrierCount]{
        barrierTransferToPresentPresentable, barrierReadToGeneralRenderTarget };

    VkResult properlyPreparedForCommands{ vkBeginCommandBuffer(commandBuffers[i],
                                                               &commandBufferBeginInfo) };
    if (properlyPreparedForCommands != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         imageMemoryBarrierCount, imageMemoryBarrierUndefinedToOperateArray);
    vkCmdCopyImage(commandBuffers[i],
                   renderTargetImages[i].handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   presentableImages[i].handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &copyRegion);
    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         imageMemoryBarrierCount, imageMemoryBarrierOperateToPresentArray);

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
