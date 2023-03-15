
#include "Instance.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FInstance::Create(const FInstanceAttributes& attributes)
{
  m_Attributes = attributes;

  VkApplicationInfo applicationInfo{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "UncannyEngineApplication",
    .applicationVersion = 1,
    .pEngineName = "UGraphicsEngine",
    .engineVersion = 1,
    .apiVersion = m_Attributes.GetVersion()
  };

  const auto& requestedLayers = m_Attributes.GetRequestedLayers();
  const auto& requestedExtensions = m_Attributes.GetRequestedExtensions();

  VkInstanceCreateInfo instanceInfo{
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = static_cast<u32>(requestedLayers.size()),
    .ppEnabledLayerNames = requestedLayers.data(),
    .enabledExtensionCount = static_cast<u32>(requestedExtensions.size()),
    .ppEnabledExtensionNames = requestedExtensions.data()
  };

  VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);
  AssertVkAndThrow(result);
}


void FInstance::Destroy()
{
  if (m_Instance != VK_NULL_HANDLE)
  {
    vkDestroyInstance(m_Instance, nullptr);
  }
}


std::vector<VkPhysicalDevice> FInstance::QueryAvailablePhysicalDevices() const
{
  u32 count{ 0 };
  VkResult result = vkEnumeratePhysicalDevices(m_Instance, &count, nullptr);
  AssertVkAndThrow(result);
  std::vector<VkPhysicalDevice> physicalDevicesVector(count);
  result = vkEnumeratePhysicalDevices(m_Instance, &count, physicalDevicesVector.data());
  AssertVkAndThrow(result);
  return physicalDevicesVector;
}


}
