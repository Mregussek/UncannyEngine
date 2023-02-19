
#include "RenderHardwareInterfaceVulkan.h"
#include <volk.h>


namespace uncanny {


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan() {
  Destroy();
}


void FRenderHardwareInterfaceVulkan::Create() {
  m_VolkHandler.Create();

  m_InstanceProperties.Initialize();
  m_InstanceProperties.AddLayerName("VK_LAYER_KHRONOS_validation");
  m_InstanceProperties.AddExtensionName(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  m_InstanceProperties.AddExtensionName(VK_KHR_SURFACE_EXTENSION_NAME);
  m_InstanceProperties.AddExtensionName(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  m_InstanceProperties.AddExtensionName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  m_Instance.Create(m_InstanceProperties);
}


void FRenderHardwareInterfaceVulkan::Destroy() {
  m_Instance.Destroy();
  m_VolkHandler.Destroy();
}


}
