
#include "RenderHardwareInterfaceVulkan.h"
#include <volk.h>
#include "Vulkan/PhysicalDeviceSelector.h"
#include "UTools/Logger/Log.h"


namespace uncanny {


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan() {
  Destroy();
}


b8 FRenderHardwareInterfaceVulkan::Create() {
  m_Destroyed = UFALSE;

  m_VolkHandler.Create();

  m_InstanceAttributes.Initialize();
  if (!m_InstanceAttributes.IsVersionAvailable(VK_API_VERSION_1_3)) {
    FLog::critical("Not available vulkan version, cannot start RHI!");
    return UFALSE;
  }
  m_InstanceAttributes.AddLayerName("VK_LAYER_KHRONOS_validation");
  m_InstanceAttributes.AddExtensionName(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  m_InstanceAttributes.AddExtensionName(VK_KHR_SURFACE_EXTENSION_NAME);
  m_InstanceAttributes.AddExtensionName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  m_Instance.Create(m_InstanceAttributes);

  m_VolkHandler.LoadInstance(m_Instance);

  m_DebugUtils.Create(m_Instance);

  {
    auto availablePhysicalDevices = m_Instance.QueryAvailablePhysicalDevices();
    VkPhysicalDevice selectedPhysicalDevice = vulkan::FPhysicalDeviceSelector().Select(availablePhysicalDevices);
    m_PhysicalDevice.Initialize(selectedPhysicalDevice);
  }

  return UTRUE;
}


void FRenderHardwareInterfaceVulkan::Destroy() {
  if (m_Destroyed) {
    return;
  }

  m_DebugUtils.Destroy(m_Instance);
  m_Instance.Destroy();
  m_VolkHandler.Destroy();
  m_Destroyed = UTRUE;
}


}
