
#include "PhysicalDeviceAttributes.h"
#include "Utilities.h"


namespace uncanny::vulkan {


void FPhysicalDeviceAttributes::Initialize(VkPhysicalDevice physicalDevice) {
  vkGetPhysicalDeviceProperties(physicalDevice, &m_Properties);

  u32 extensionCount{ 0 };
  VkResult result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  AssertVkAndThrow(result);
  m_ExtensionProperties.resize(extensionCount);
  result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, m_ExtensionProperties.data());
  AssertVkAndThrow(result);

  u32 queueFamilyCount{ 0 };
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  m_QueueFamilyProperties.resize(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());
}


b8 FPhysicalDeviceAttributes::IsExtensionPresent(const char* extensionName) const {
  auto it = std::ranges::find_if(m_ExtensionProperties, [extensionName](const VkExtensionProperties& extensionProperties) -> b32 {
    return std::strcmp(extensionName, extensionProperties.extensionName) == 0;
  });
  if (it == m_ExtensionProperties.end()) {
    return UFALSE;
  }

  return UTRUE;
}


}
