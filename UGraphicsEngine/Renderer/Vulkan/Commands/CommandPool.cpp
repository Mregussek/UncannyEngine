
#include "CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FCommandPool::Create(FQueueFamilyIndex queueFamilyIndex, VkDevice vkDevice)
{
  m_QueueFamilyIndex = queueFamilyIndex;
  m_Device = vkDevice;

  VkCommandPoolCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = m_QueueFamilyIndex
  };

  VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool);
  AssertVkAndThrow(result);
}


void FCommandPool::Destroy()
{
  if (m_CommandPool != VK_NULL_HANDLE)
  {
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
  }
}


void FCommandPool::Reset()
{
  VkCommandPoolResetFlags flags = 0;
  VkResult result = vkResetCommandPool(m_Device, m_CommandPool, flags);
  AssertVkAndThrow(result);
}


std::vector<FCommandBuffer> FCommandPool::AllocatePrimaryCommandBuffers(u32 count) const
{
  VkCommandBufferAllocateInfo allocateInfo{
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = nullptr,
    .commandPool = m_CommandPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = count
  };

  std::vector<VkCommandBuffer> vkCommandBuffers(allocateInfo.commandBufferCount);
  VkResult result = vkAllocateCommandBuffers(m_Device, &allocateInfo, vkCommandBuffers.data());
  AssertVkAndThrow(result);

  std::vector<FCommandBuffer> rtnCommandBuffers{};
  rtnCommandBuffers.reserve(allocateInfo.commandBufferCount);
  for(VkCommandBuffer vcb : vkCommandBuffers)
  {
    rtnCommandBuffers.emplace_back(m_Device, m_CommandPool, vcb);
  }

  return rtnCommandBuffers;
}


FCommandBuffer FCommandPool::AllocateCommandBuffer() const
{
  VkCommandBufferAllocateInfo allocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = m_CommandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1
  };

  VkCommandBuffer vkCommandBuffer{ VK_NULL_HANDLE };
  VkResult result = vkAllocateCommandBuffers(m_Device, &allocateInfo, &vkCommandBuffer);
  AssertVkAndThrow(result);

  return { m_Device, m_CommandPool, vkCommandBuffer };
}


}
