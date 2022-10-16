
#include "Semaphore.h"
#include "Utilities.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


b32 FSemaphoreVulkan::init(const FSemaphoreInitDependenciesVulkan& deps) {
  UTRACE("Initializing semaphore vulkan...");

  VkSemaphoreCreateInfo createInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;

  AssertResultVulkan( vkCreateSemaphore(deps.device, &createInfo, nullptr, &m_VkSemaphore) );

  UDEBUG("Initialized semaphore vulkan!");
  return UTRUE;
}


void FSemaphoreVulkan::terminate(VkDevice device) {
  UTRACE("Terminating semaphore vulkan...");

  if (m_VkSemaphore != VK_NULL_HANDLE) {
    UTRACE("Destroying semaphore...");
    vkDestroySemaphore(device, m_VkSemaphore, nullptr);
    m_VkSemaphore = VK_NULL_HANDLE;
  }
  else {
    UWARN("As semaphore is not created, so it is not destroyed!");
  }

  UDEBUG("Terminated semaphore vulkan!");
}


}
