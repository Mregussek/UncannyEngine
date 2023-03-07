
#include "Instance.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FInstance::Create(const FInstanceAttributes& attributes)
{
  m_Attributes = attributes;

  VkApplicationInfo applicationInfo{};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.pNext = nullptr;
  applicationInfo.pApplicationName = "UncannyEngineApplication";
  applicationInfo.applicationVersion = 1;
  applicationInfo.pEngineName = "UGraphicsEngine";
  applicationInfo.engineVersion = 1;
  applicationInfo.apiVersion = m_Attributes.GetVersion();

  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = nullptr;
  instanceInfo.flags = 0;
  instanceInfo.pApplicationInfo = &applicationInfo;
  instanceInfo.enabledLayerCount = m_Attributes.GetRequestedLayers().size();
  instanceInfo.ppEnabledLayerNames = m_Attributes.GetRequestedLayers().data();
  instanceInfo.enabledExtensionCount = m_Attributes.GetRequestedExtensions().size();
  instanceInfo.ppEnabledExtensionNames = m_Attributes.GetRequestedExtensions().data();
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
