
#include "PhysicalDevice.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


void FPhysicalDevice::Initialize(VkPhysicalDevice physicalDevice)
{
  m_PhysicalDevice = physicalDevice;

  m_Attributes.Initialize(m_PhysicalDevice);

  UINFO("Chosen Physical Device: {}", m_Attributes.GetDeviceProperties().deviceName);
  UINFO("Vulkan SDK Version: {}.{}.{}", VK_API_VERSION_MAJOR(m_Attributes.GetDeviceProperties().apiVersion),
        VK_API_VERSION_MINOR(m_Attributes.GetDeviceProperties().apiVersion),
        VK_API_VERSION_PATCH(m_Attributes.GetDeviceProperties().apiVersion));
}


}
