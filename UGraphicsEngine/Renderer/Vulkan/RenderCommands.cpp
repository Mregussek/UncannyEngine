
#include "RenderCommands.h"
#include <algorithm>


namespace uncanny::vulkan
{


void FRenderCommands::RecordClearColorImage(std::vector<FCommandBuffer>& commandBuffers,
                                            std::span<const VkImage> images, VkClearColorValue clearColorValue,
                                            VkImageLayout finalLayout)
{
  std::ranges::for_each(commandBuffers,
                        [images, idx = 0, clearColorValue, finalLayout](FCommandBuffer& cmdBuf) mutable
  {
    VkImageSubresourceRange subresourceRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };
    VkImage image = images[idx];

    cmdBuf.BeginRecording();
    cmdBuf.ImageMemoryBarrier(image,
                              VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    cmdBuf.ClearColorImage(image, clearColorValue, subresourceRange);
    cmdBuf.ImageMemoryBarrier(image,
                              VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, finalLayout,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    cmdBuf.EndRecording();

    idx++;
  });
}


void FRenderCommands::RecordCopyImage(std::vector<FCommandBuffer>& commandBuffers, std::span<const VkImage> srcImages,
                                      std::span<const VkImage> dstImages, VkExtent2D extent, VkImageLayout finalLayout)
{
  std::ranges::for_each(commandBuffers,
                        [srcImages, dstImages, extent, finalLayout, idx = 0](FCommandBuffer& cmdBuf) mutable
  {
    VkImageSubresourceRange subresourceRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };
    VkImageSubresourceLayers subresourceLayers{
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1
    };
    VkImage srcImage = srcImages[idx];
    VkImage dstImage = dstImages[idx];

    cmdBuf.BeginRecording();
    cmdBuf.ImageMemoryBarrier(dstImage,
                              VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    cmdBuf.CopyImage(srcImage, dstImage, subresourceLayers, extent);
    cmdBuf.ImageMemoryBarrier(dstImage,
                              VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, finalLayout,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    cmdBuf.EndRecording();
    idx++;
  });
}


}
