
#include "CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include <algorithm>


namespace uncanny::vulkan {


void FCommandPool::Create(FQueueFamilyIndex queueFamilyIndex, VkDevice vkDevice)
{
  m_Device = vkDevice;

  VkCommandPoolCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  createInfo.queueFamilyIndex = queueFamilyIndex;

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
  VkCommandBufferAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = m_CommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = count;

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


FCommandBuffer FCommandPool::AllocateAndBeginSingleUseCommandBuffer() const
{
  VkCommandBufferAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = m_CommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = 1;

  VkCommandBuffer vkCommandBuffer{ VK_NULL_HANDLE };
  VkResult result = vkAllocateCommandBuffers(m_Device, &allocateInfo, &vkCommandBuffer);
  AssertVkAndThrow(result);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.pNext = nullptr;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  result = vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
  AssertVkAndThrow(result);

  return { m_Device, m_CommandPool, vkCommandBuffer };
}


}
