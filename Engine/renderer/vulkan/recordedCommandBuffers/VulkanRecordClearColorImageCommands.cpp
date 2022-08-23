
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

  b32 recordedClearScreen{
      recordCommandBuffersForClearingColorImage(mRenderTargetImageVector,
                                                mVkRenderCommandBufferVector) };
  if (not recordedClearScreen) {
    UFATAL("Could not record clear screen command buffers!");
    return UFALSE;
  }

  b32 recordedCopyImage{
      recordCommandBuffersForCopyRenderTargetIntoPresentableImage(mRenderTargetImageVector,
                                                                  mVkImagePresentableVector,
                                                                  mVkCopyCommandBufferVector) };
  if (not recordedCopyImage) {
    UFATAL("Could not record copy image command buffers!");
    return UFALSE;
  }

  return UTRUE;
}


b32 FRenderContextVulkan::recordCommandBuffersForClearingColorImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<VkCommandBuffer>& commandBuffers) {
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
  barrierClearToGeneral.newLayout = VK_IMAGE_LAYOUT_GENERAL;
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


b32 FRenderContextVulkan::recordCommandBuffersForCopyRenderTargetIntoPresentableImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<VkImage>& presentableImages,
    const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers for copying render target images into presentable ones!");

  VkImageSubresourceLayers subresourceLayers{};
  subresourceLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceLayers.mipLevel = 0;
  subresourceLayers.baseArrayLayer = 0;
  subresourceLayers.layerCount = 1;

  VkOffset3D offset{};
  offset.x = 0;
  offset.y = 0;
  offset.z = 0;

  VkImageCopy region{};
  region.srcSubresource = subresourceLayers;
  region.srcOffset = offset;
  region.dstSubresource = subresourceLayers;
  region.dstOffset = offset;
  region.extent = renderTargetImages[0].extent;

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

  VkImageMemoryBarrier barrierUndefinedToTransferRenderTarget{ defaultBarrier };
  barrierUndefinedToTransferRenderTarget.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierUndefinedToTransferRenderTarget.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierUndefinedToTransferRenderTarget.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrierUndefinedToTransferRenderTarget.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrierUndefinedToTransferRenderTarget.image = VK_NULL_HANDLE; // will fill later

  VkImageMemoryBarrier barrierTransferToGeneralRenderTarget{ defaultBarrier };
  barrierTransferToGeneralRenderTarget.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierTransferToGeneralRenderTarget.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierTransferToGeneralRenderTarget.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrierTransferToGeneralRenderTarget.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
  barrierTransferToGeneralRenderTarget.image = VK_NULL_HANDLE; // will fill later

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
    barrierUndefinedToTransferPresentable.image = presentableImages[i];
    barrierTransferToPresentPresentable.image = presentableImages[i];

    barrierUndefinedToTransferRenderTarget.image = renderTargetImages[i].handle;
    barrierTransferToGeneralRenderTarget.image = renderTargetImages[i].handle;


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
                         1, &barrierUndefinedToTransferRenderTarget);
    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         1, &barrierUndefinedToTransferPresentable);
    vkCmdCopyImage(commandBuffers[i],
                   renderTargetImages[i].handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   presentableImages[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &region);
    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         1, &barrierTransferToGeneralRenderTarget);
    vkCmdPipelineBarrier(commandBuffers[i],
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VkDependencyFlags{ 0 },
                         0, nullptr,
                         0, nullptr,
                         1, &barrierTransferToPresentPresentable);

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
