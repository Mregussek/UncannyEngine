
#include "CommandPoolFactory.h"
#include "CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include <algorithm>


namespace uncanny::vulkan {


std::vector<FCommandBuffer> FCommandPoolFactory::AllocatePrimaryCommandBuffers(u32 count) const {
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
  std::ranges::for_each(vkCommandBuffers, [vkDevice = m_Device, &rtnCommandBuffers](VkCommandBuffer vkCommandBuffer){
    rtnCommandBuffers.emplace_back(vkDevice, vkCommandBuffer);
  });
  return rtnCommandBuffers;
}


FCommandBuffer FCommandPoolFactory::AllocateAndBeginSingleUseCommandBuffer() const {
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

  return { m_Device, vkCommandBuffer };
}


}
