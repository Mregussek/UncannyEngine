
#include "PhysicalDevice.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan {


void FPhysicalDevice::Initialize(VkPhysicalDevice physicalDevice) {
  m_PhysicalDevice = physicalDevice;

  m_Attributes.Initialize(m_PhysicalDevice);

  FLog::info("Chosen Physical Device: {}", m_Attributes.GetDeviceProperties().deviceName);
}


}
