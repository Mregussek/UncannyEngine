
#include "Semaphore.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FSemaphore::Create(VkDevice vkDevice)
{
  m_Device = vkDevice;

  VkSemaphoreCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;

  VkResult result = vkCreateSemaphore(m_Device, &createInfo, nullptr, &m_Semaphore);
  AssertVkAndThrow(result);
}


void FSemaphore::Destroy()
{
  if (m_Semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_Device, m_Semaphore, nullptr);
  }
}


}
