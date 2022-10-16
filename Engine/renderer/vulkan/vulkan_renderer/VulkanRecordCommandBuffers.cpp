
#include "RendererVulkan.h"
#include "VulkanRecordCommandBuffers.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 recordCopyRenderTargetIntoPresentableImage(
    const std::vector<vkf::FImageVulkan>& renderTargetImages,
    const std::vector<vkf::FImageVulkan>& presentableImages,
    const std::vector<VkCommandBuffer>& commandBuffers) {
  UTRACE("Recording command buffers for copying render target images into presentable ones!");

  if constexpr (ENABLE_DEBUGGING_RENDERER) {
    UTRACE("Making sure that copy render target into presentable command buffers can be recorded!");
    if (renderTargetImages.size() == presentableImages.size() and renderTargetImages.empty()) {
      UERROR("No render target images and no presentable images! Cannot record cmd!");
      return UFALSE;
    }

    for (u32 i = 0; i < renderTargetImages.size(); i++) {
      const vkf::FImageDataVulkan& renderData{ renderTargetImages[i].getData() };
      const vkf::FImageDataVulkan& presentData{ presentableImages[i].getData() };

      if (renderData.type != vkf::EImageType::RENDER_TARGET) {
        UERROR("Render target image has unsupported type: {}", (i32)renderData.type);
        return UFALSE;
      }
      if (presentData.type != vkf::EImageType::PRESENTABLE) {
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
