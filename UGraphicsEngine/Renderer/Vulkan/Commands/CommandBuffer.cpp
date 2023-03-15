
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

  VkCommandBufferBeginInfo beginInfo{
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pInheritanceInfo = nullptr
  };

  VkResult result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
  AssertVkAndThrow(result);
}


void FCommandBuffer::BeginOneTimeRecording()
{
  if (m_Recording)
  {
    UWARN("Command buffer is during recording commands, returning...");
    return;
  }
  m_Recording = UTRUE;

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr
  };

  VkResult result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
  AssertVkAndThrow(result);
}


void FCommandBuffer::EndRecording()
{
  m_Recording = UFALSE;

  VkResult result = vkEndCommandBuffer(m_CommandBuffer);
  AssertVkAndThrow(result);
}


void FCommandBuffer::ImageMemoryBarrier(VkImage image, VkAccessFlags srcFlags, VkAccessFlags dstFlags,
                                        VkImageLayout oldLayout, VkImageLayout newLayout,
                                        VkImageSubresourceRange subresourceRange,
                                        VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage)
{
  VkImageMemoryBarrier barrier{
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = srcFlags,
    .dstAccessMask = dstFlags,
    .oldLayout = oldLayout,
    .newLayout = newLayout,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = image,
    .subresourceRange = subresourceRange
  };

  vkCmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, VkDependencyFlags{ 0 },
                       0, nullptr, 0, nullptr, 1, &barrier);
}


void FCommandBuffer::ClearColorImage(VkImage image, VkClearColorValue clearValue,
                                     VkImageSubresourceRange subresourceRange)
{
  vkCmdClearColorImage(m_CommandBuffer,
                       image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       &clearValue,
                       1, &subresourceRange);
}


}
