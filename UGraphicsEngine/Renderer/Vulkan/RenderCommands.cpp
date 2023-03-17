
#include "RenderCommands.h"
#include <algorithm>


namespace uncanny::vulkan
{


void FRenderCommands::RecordClearColorImage(std::vector<FCommandBuffer>& commandBuffers,
                                            const std::vector<VkImage>& images,
                                            VkClearColorValue clearColorValue)
{
  VkImageSubresourceRange subresourceRange{
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
  };

  std::ranges::for_each(commandBuffers,
                        [images, idx = 0, &subresourceRange, clearColorValue](FCommandBuffer& cmdBuf) mutable
  {
    VkImage image = images[idx];

    cmdBuf.BeginRecording();
    cmdBuf.ImageMemoryBarrier(image,
                              VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    cmdBuf.ClearColorImage(image, clearColorValue, subresourceRange);
    cmdBuf.ImageMemoryBarrier(image,
                              VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    cmdBuf.EndRecording();

    idx++;
  });
}


}
