
#include "RendererVulkan.h"
#include <renderer/vulkan/vulkan_framework/Utilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createCommandPool() {
  UTRACE("Creating command pools...");

  // even when VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT is set use vkResetCommandPool
  VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
  createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  createInfo.queueFamilyIndex = m_PhysicalDevice.QueueFamilyGraphics();

  vkf::AssertResultVulkan( vkCreateCommandPool(m_LogicalDevice.Handle(), &createInfo, nullptr,
                                               &mVkGraphicsCommandPool) );

  createInfo.queueFamilyIndex = m_PhysicalDevice.QueueFamilyTransfer();

  vkf::AssertResultVulkan( vkCreateCommandPool(m_LogicalDevice.Handle(), &createInfo, nullptr,
                                   &mVkTransferCommandPool) );

  UDEBUG("Created command pools!");
  return UTRUE;
}


b32 FRendererVulkan::resetCommandPool(const VkCommandPool& commandPool) {
  UTRACE("Resetting command pools...");

  VkCommandPoolResetFlags flags = 0;
  VkResult result{ vkResetCommandPool(m_LogicalDevice.Handle(), commandPool, flags) };
  if (result != VK_SUCCESS) {
    UERROR("Could not reset command pools!");
    return UFALSE;
  }

  UDEBUG("Reset command pools!");
  return UTRUE;
}


b32 FRendererVulkan::closeCommandPool() {
  UTRACE("Closing command pools...");

  if (mVkGraphicsCommandPool != VK_NULL_HANDLE) {
    UTRACE("Destroying graphics command pool...");
    vkDestroyCommandPool(m_LogicalDevice.Handle(), mVkGraphicsCommandPool, nullptr);
  }
  else {
    UWARN("As graphics command pool was not created, it won't be closed!");
  }

  if (mVkTransferCommandPool != VK_NULL_HANDLE) {
    UTRACE("Destroying transfer command pool...");
    vkDestroyCommandPool(m_LogicalDevice.Handle(), mVkTransferCommandPool, nullptr);
  }
  else {
    UWARN("As transfer command pool was not created, it won't be closed!");
  }

  UDEBUG("Closed command pools!");
  return UTRUE;
}


}
