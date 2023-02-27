
#include "LogicalDeviceFactory.h"
#include "UGraphicsEngine/RHI/Vulkan/Utilities.h"


namespace uncanny::vulkan {


FCommandPool FLogicalDeviceFactory::CreateCommandPool(FQueueFamilyIndex queueFamilyIndex) {
  VkCommandPoolCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  createInfo.queueFamilyIndex = queueFamilyIndex;

  VkCommandPool vkCommandPool{ VK_NULL_HANDLE };
  VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &vkCommandPool);
  AssertVkAndThrow(result);

  return FCommandPool{ m_Device, vkCommandPool };
}


}
