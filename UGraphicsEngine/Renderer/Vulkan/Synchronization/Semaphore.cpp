
#include "Semaphore.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FSemaphore::~FSemaphore()
{
  Destroy();
}


void FSemaphore::Create(VkDevice vkDevice)
{
  m_Device = vkDevice;

  VkSemaphoreCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0
  };

  VkResult result = vkCreateSemaphore(m_Device, &createInfo, nullptr, &m_Semaphore);
  AssertVkAndThrow(result);
}


void FSemaphore::Destroy()
{
  if (m_Semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_Device, m_Semaphore, nullptr);
    m_Semaphore = VK_NULL_HANDLE;
  }
}


}
