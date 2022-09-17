
#include "RendererVulkan.h"
#include "VulkanRecordCommandBuffers.h"
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 recordIndexedVertexBufferGraphicsPipelineForRenderTarget(
    const FRecordCommandsForIndexVertexBuffersDependencies& deps) {
  UTRACE("Recording command buffers with indexed vertex buffer binding in graphics pipeline for"
         "render target images...");

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkRect2D renderArea{};
  renderArea.extent = {}; // will be filled later
  renderArea.offset = { 0, 0 };

  VkClearValue clearColorValue{};
  clearColorValue.color = { 0.2f, 0.5f, 0.8f, 0.0f };
  VkClearValue clearDepthValue{};
  clearDepthValue.depthStencil = { 1.f, 0 };
  std::array<VkClearValue, 2> clearValues{ clearColorValue, clearDepthValue };

  VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  renderPassBeginInfo.pNext = nullptr;
  renderPassBeginInfo.renderPass = deps.renderPass;
  renderPassBeginInfo.framebuffer = VK_NULL_HANDLE; // will be filled later
  renderPassBeginInfo.renderArea = {}; // will be filled later
  renderPassBeginInfo.clearValueCount = clearValues.size();
  renderPassBeginInfo.pClearValues = clearValues.data();

  VkDeviceSize vertexBufferOffsets[]{ 0 };
  VkBuffer vertexHandle{ deps.pVertexBuffer->getData().handle };
  VkBuffer indexHandle{ deps.pIndexBuffer->getData().handle };
  u32 indicesCount{ deps.pIndexBuffer->getData().elemCount };
  u32 renderTargetsSize{ (u32)deps.pRenderTargets->size() };
  const std::vector<VkDescriptorSet>& descriptorSets{ deps.pGraphicsPipeline->getDescriptorSets() };

  for (u32 i = 0; i < renderTargetsSize; i++) {
    renderArea.extent.width = deps.pRenderTargets->at(i).getData().extent.width;
    renderArea.extent.height = deps.pRenderTargets->at(i).getData().extent.height;
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.framebuffer = deps.pRenderTargets->at(i).getData().handleFramebuffer;

    VkCommandBuffer cmdBuffer{ deps.pCommandBuffers->at(i) };

    VkResult properlyBegan{ vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo) };
    if (properlyBegan != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      deps.pGraphicsPipeline->getPipelineHandle());
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexHandle, vertexBufferOffsets);
    vkCmdBindIndexBuffer(cmdBuffer, indexHandle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdSetViewport(cmdBuffer, 0, 1, &deps.viewport);
    vkCmdSetScissor(cmdBuffer, 0, 1, &deps.scissor);
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            deps.pGraphicsPipeline->getPipelineLayout(), 0,
                            descriptorSets.size(), descriptorSets.data(),
                            0, nullptr);
    vkCmdDrawIndexed(cmdBuffer, indicesCount, 1, 0, 0, 0);

    vkCmdEndRenderPass(cmdBuffer);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(cmdBuffer) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Recorded command buffers with indexed vertex buffer binding in graphics pipeline for "
         "render target images!");
  return UTRUE;
}


b32 recordCopyRenderTargetIntoPresentableImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<FImageVulkan>& presentableImages,
    const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers for copying render target images into presentable ones!");

  if constexpr (U_VK_DEBUG) {
    UTRACE("Making sure that copy render target into presentable command buffers can be recorded!");
    if (renderTargetImages.size() == presentableImages.size() and renderTargetImages.empty()) {
      UERROR("No render target images and no presentable images! Cannot record cmd!");
      return UFALSE;
    }

    for (u32 i = 0; i < renderTargetImages.size(); i++) {
      const FImageDataVulkan& renderData{ renderTargetImages[i].getData() };
      const FImageDataVulkan& presentData{ presentableImages[i].getData() };

      if (renderData.type != EImageType::RENDER_TARGET) {
        UERROR("Render target image has unsupported type: {}", (i32)renderData.type);
        return UFALSE;
      }
      if (presentData.type != EImageType::PRESENTABLE) {
        UERROR("Render target image has unsupported type: {}", (i32)renderData.type);
        return UFALSE;
      }
      if (renderData.format != presentData.format) {
        UERROR("Render target image and presentable image have different formats! rt {} pr {}",
               renderData.format, presentData.format);
        return UFALSE;
      }
      if (renderData.extent.height != presentData.extent.height) {
        UERROR("Render target image height does not equal to presentable one's height! rt {} pr {}",
               renderData.extent.height, presentData.extent.height);
        return UFALSE;
      }
      if (renderData.extent.width != presentData.extent.width) {
        UERROR("Render target image width does not equal to presentable one's width! rt {} pr {}",
               renderData.extent.width, presentData.extent.width);
        return UFALSE;
      }
      if (renderData.extent.depth != presentData.extent.depth) {
        UERROR("Render target image width does not equal to presentable one's depth! rt {} pr {}",
               renderData.extent.depth, presentData.extent.depth);
        return UFALSE;
      }
      if (renderData.extent.depth != 1) {
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
  copyRegion.extent = renderTargetImages[0].getData().extent;

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
    VkImage presentImageHandle{ presentableImages[i].getData().handle };
    VkImage renderImageHandle{ renderTargetImages[i].getData().handle };

    barrierPresentableUndefinedToTransfer.image = presentImageHandle;
    barrierPresentableTransferToPresent.image = presentImageHandle;

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
                   renderImageHandle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   presentImageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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


}
