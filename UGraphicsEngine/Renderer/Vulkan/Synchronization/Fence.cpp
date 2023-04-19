
#include "Fence.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


FFence::~FFence()
{
  Destroy();
}


void FFence::Create(VkDevice vkDevice, VkFenceCreateFlags flags)
{
  m_Device = vkDevice;

  VkFenceCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags
  };
  VkResult result = vkCreateFence(m_Device, &createInfo, nullptr, &m_Fence);
  AssertVkAndThrow(result);
}


void FFence::Destroy()
{
  if (m_Fence != VK_NULL_HANDLE)
  {
    vkDestroyFence(m_Device, m_Fence, nullptr);
    m_Fence = VK_NULL_HANDLE;
  }
}


void FFence::WaitAndReset() const {
  VkBool32 waitForAll{ VK_TRUE };
  u64 timeout{ std::numeric_limits<u64>::max() };
  vkWaitForFences(m_Device, 1, &m_Fence, waitForAll, timeout);
  vkResetFences(m_Device, 1, &m_Fence);
}


}
