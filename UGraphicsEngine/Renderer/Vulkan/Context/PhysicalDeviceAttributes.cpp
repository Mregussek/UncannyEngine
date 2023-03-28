
#include "PhysicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FPhysicalDeviceAttributes::Initialize(VkPhysicalDevice vkPhysicalDevice)
{
  m_PhysicalDevice = vkPhysicalDevice;

  vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);

  u32 extensionCount{ 0 };
  VkResult result = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);
  AssertVkAndThrow(result);
  m_ExtensionProperties.resize(extensionCount);
  result = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount,
                                                m_ExtensionProperties.data());
  AssertVkAndThrow(result);

  u32 queueFamilyCount{ 0 };
  vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
  m_QueueFamilyProperties.resize(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

  vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
  vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);
}


b8 FPhysicalDeviceAttributes::IsExtensionPresent(const char* extensionName) const {
  auto it = std::ranges::find_if(m_ExtensionProperties,
                                 [extensionName](const VkExtensionProperties& extensionProperties) -> b32
  {
    return std::strcmp(extensionName, extensionProperties.extensionName) == 0;
  });
  if (it == m_ExtensionProperties.end())
  {
    return UFALSE;
  }

  return UTRUE;
}


}
