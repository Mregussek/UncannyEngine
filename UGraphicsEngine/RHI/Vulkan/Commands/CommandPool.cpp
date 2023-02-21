
#include "CommandPool.h"
#include "UGraphicsEngine/RHI/Vulkan/Utilities.h"


namespace uncanny::vulkan {


FCommandPool::FCommandPool(VkDevice vkDevice, VkCommandPool vkCommandPool) :
  m_Device(vkDevice),
  m_CommandPool(vkCommandPool)
{
  m_Factory.m_CommandPool = m_CommandPool;
}


void FCommandPool::Destroy() {
  if (m_CommandPool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
  }
}


void FCommandPool::Reset() {
  VkCommandPoolResetFlags flags = 0;
  VkResult result = vkResetCommandPool(m_Device, m_CommandPool, flags);
  AssertVkAndThrow(result);
}


}
