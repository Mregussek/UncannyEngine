
#include "Fence.h"
#include "Utilities.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


b32 FFenceVulkan::init(const FFenceInitDependenciesVulkan& deps) {
  UTRACE("Initializing fence vulkan...");

  VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  AssertResultVulkan( vkCreateFence(deps.device, &createInfo, nullptr, &m_VkFence) );

  UDEBUG("Initialized fence vulkan!");
  return UTRUE;
}


void FFenceVulkan::terminate(VkDevice device) {
  UTRACE("Terminating fence vulkan...");

  if (m_VkFence != VK_NULL_HANDLE) {
    UTRACE("Destroying fence...");
    vkDestroyFence(device, m_VkFence, nullptr);
    m_VkFence = VK_NULL_HANDLE;
  }
  else {
    UWARN("As fence is not created, so it is not destroyed!");
  }

  UDEBUG("Terminated fence vulkan!");
}


void FFenceVulkan::wait(VkDevice device) const {
  UTRACE("Waiting for fence...");
  vkWaitForFences(device, 1, &m_VkFence, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &m_VkFence);
}


}
