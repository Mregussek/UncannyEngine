
#include "CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


FCommandBuffer::FCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBuffer vkCommandBuffer)
  : m_CommandBuffer(vkCommandBuffer),
  m_Device(vkDevice),
  m_CommandPool(vkCommandPool)
{
}


FCommandBuffer::~FCommandBuffer()
{
  Free();
}


void FCommandBuffer::Free()
{
  if (m_Freed)
  {
    return;
  }

  if (m_CommandBuffer != VK_NULL_HANDLE)
  {
    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CommandBuffer);
    m_Freed = UTRUE;
  }
}


void FCommandBuffer::BeginRecording()
{
  if (m_Recording)
  {
    UWARN("Command buffer is during recording commands, returning...");
    return;
  }
  m_Recording = UTRUE;

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.pNext = nullptr;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  VkResult result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
  AssertVkAndThrow(result);
}


void FCommandBuffer::EndRecording()
{
  m_Recording = UFALSE;

  VkResult result = vkEndCommandBuffer(m_CommandBuffer);
  AssertVkAndThrow(result);
}


void FCommandBuffer::ImageMemoryBarrierToStartTransfer(VkImage image) const
{
  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.pNext = nullptr;
  barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange = imageSubresourceRange;

  vkCmdPipelineBarrier(m_CommandBuffer,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VkDependencyFlags{ 0 },
                       0, nullptr,
                       0, nullptr,
                       1, &barrier);
}


void FCommandBuffer::ImageMemoryBarrierToFinishTransferAndStartPresentation(VkImage image) const
{
  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.pNext = nullptr;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange = imageSubresourceRange;

  vkCmdPipelineBarrier(m_CommandBuffer,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                       VkDependencyFlags{ 0 },
                       0, nullptr,
                       0, nullptr,
                       1, &barrier);
}


}
