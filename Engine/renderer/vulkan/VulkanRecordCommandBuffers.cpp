
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 recordClearColorImage(const std::vector<FImageVulkan>& renderTargetImages,
                          const std::vector<VkCommandBuffer>& commandBuffers,
                          u32 queueFamilyIndex) {
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
  barrierUnknownToClear.srcQueueFamilyIndex = queueFamilyIndex;
  barrierUnknownToClear.dstQueueFamilyIndex = queueFamilyIndex;
  barrierUnknownToClear.image = VK_NULL_HANDLE; // will be filled later
  barrierUnknownToClear.subresourceRange = imageSubresourceRange;

  VkImageMemoryBarrier barrierClearToGeneral{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
  barrierClearToGeneral.pNext = nullptr;
  barrierClearToGeneral.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierClearToGeneral.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierClearToGeneral.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierClearToGeneral.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrierClearToGeneral.srcQueueFamilyIndex = queueFamilyIndex;
  barrierClearToGeneral.dstQueueFamilyIndex = queueFamilyIndex;
  barrierClearToGeneral.image = VK_NULL_HANDLE; // will be filled later
  barrierClearToGeneral.subresourceRange = imageSubresourceRange;

  VkClearColorValue clearColor{ 1.0f, 0.8f, 0.4f, 0.0f };

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


b32 recordCopyRenderTargetIntoPresentableImage(const std::vector<FImageVulkan>& renderTargetImages,
                                               const std::vector<FImageVulkan>& presentableImages,
                                               const std::vector<VkCommandBuffer>& commandBuffers,
                                               u32 queueFamilyIndex) {
  UTRACE("Recording command buffers for copying render target images into presentable ones!");

  if constexpr (U_VK_DEBUG) {
    UTRACE("Making sure that copy render target into presentable command buffers can be recorded!");
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
      if (renderTargetImages[i].extent.depth != presentableImages[i].extent.depth) {
        UERROR("Render target image width does not equal to presentable one's depth! rt {} pr {}",
               renderTargetImages[i].extent.depth, presentableImages[i].extent.depth);
        return UFALSE;
      }
      if (renderTargetImages[i].extent.depth != 1) {
        UERROR("Render target and presentable image have depth other than 1!");
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
  defaultBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  defaultBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  defaultBarrier.subresourceRange = imageSubresourceRange;

  VkImageMemoryBarrier barrierPresentableUndefinedToTransfer{ defaultBarrier };
  barrierPresentableUndefinedToTransfer.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierPresentableUndefinedToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierPresentableUndefinedToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrierPresentableUndefinedToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierPresentableUndefinedToTransfer.image = VK_NULL_HANDLE; // will fill later

  VkImageMemoryBarrier barrierPresentableTransferToPresent{ defaultBarrier };
  barrierPresentableTransferToPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrierPresentableTransferToPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrierPresentableTransferToPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrierPresentableTransferToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrierPresentableTransferToPresent.image = VK_NULL_HANDLE; // will fill later

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    barrierPresentableUndefinedToTransfer.image = presentableImages[i].handle;
    barrierPresentableTransferToPresent.image = presentableImages[i].handle;

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
                         1, &barrierPresentableUndefinedToTransfer);
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
                         1, &barrierPresentableTransferToPresent);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(commandBuffers[i]) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Properly recorded command buffers for copying render targets into presentable images!");
  return UTRUE;
}


b32 recordClearScreenWithRenderPassForRenderTarget(
    const std::vector<FImageVulkan>& renderTargetImages, VkRenderPass renderPass,
    const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers with clearing screen with render pass for render target...");

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkRect2D renderArea{};
  renderArea.extent = {}; // will be filled later
  renderArea.offset = { 0, 0 };

  VkClearValue clearColorValue{ 1.0f, 0.3f, 0.8f, 0.0f };

  VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  renderPassBeginInfo.pNext = nullptr;
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.framebuffer = VK_NULL_HANDLE; // will be filled later
  renderPassBeginInfo.renderArea = {}; // will be filled later
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColorValue;

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    renderArea.extent.width = renderTargetImages[i].extent.width;
    renderArea.extent.height = renderTargetImages[i].extent.height;
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.framebuffer = renderTargetImages[i].handleFramebuffer;

    VkResult properlyPreparedForCommands{ vkBeginCommandBuffer(commandBuffers[i],
                                                               &commandBufferBeginInfo) };
    if (properlyPreparedForCommands != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffers[i]);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(commandBuffers[i]) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Recorded command buffers with clearing screen with render pass for render target!");
  return UTRUE;
}


b32 recordGraphicsPipelineForRenderTarget(const std::vector<FImageVulkan>& renderTargetImages,
                                          VkRenderPass renderPass, VkPipeline graphicsPipeline,
                                          const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers with graphics pipeline usage for render target images...");

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkRect2D renderArea{};
  renderArea.extent = {}; // will be filled later
  renderArea.offset = { 0, 0 };

  VkClearValue clearColorValue{ 0.5f, 0.9f, 0.3f, 0.0f };

  VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  renderPassBeginInfo.pNext = nullptr;
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.framebuffer = VK_NULL_HANDLE; // will be filled later
  renderPassBeginInfo.renderArea = {}; // will be filled later
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColorValue;

  for (u32 i = 0; i < renderTargetImages.size(); i++) {
    renderArea.extent.width = renderTargetImages[i].extent.width;
    renderArea.extent.height = renderTargetImages[i].extent.height;
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.framebuffer = renderTargetImages[i].handleFramebuffer;

    VkResult properlyPreparedForCommands{ vkBeginCommandBuffer(commandBuffers[i],
                                                               &commandBufferBeginInfo) };
    if (properlyPreparedForCommands != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(commandBuffers[i]) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Recorded command buffers with graphics pipeline usage for render target images!");
  return UTRUE;
}


}
