
#include "RenderHardwareInterfaceVulkan.h"
#include <volk.h>
#include "UTools/Logger/Log.h"


namespace uncanny {


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan() {
  Destroy();
}


b8 FRenderHardwareInterfaceVulkan::Create() {
  m_Destroyed = UFALSE;

  m_VolkHandler.Create();

  m_InstanceProperties.Initialize();
  if (!m_InstanceProperties.IsVersionAvailable(VK_API_VERSION_1_3)) {
    FLog::critical("Not available vulkan version, cannot start RHI!");
    return UFALSE;
  }
  m_InstanceProperties.AddLayerName("VK_LAYER_KHRONOS_validation");
  m_InstanceProperties.AddExtensionName(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  m_InstanceProperties.AddExtensionName(VK_KHR_SURFACE_EXTENSION_NAME);
  m_InstanceProperties.AddExtensionName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  m_Instance.Create(m_InstanceProperties);

  m_VolkHandler.LoadInstance(m_Instance);

  return UTRUE;
}


void FRenderHardwareInterfaceVulkan::Destroy() {
  if (m_Destroyed) {
    return;
  }

  m_Instance.Destroy();
  m_VolkHandler.Destroy();
  m_Destroyed = UTRUE;
}


}
