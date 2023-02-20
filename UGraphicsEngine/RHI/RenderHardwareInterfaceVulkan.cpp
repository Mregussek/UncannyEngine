
#include "RenderHardwareInterfaceVulkan.h"
#include <volk.h>
#include "Vulkan/InstanceAttributes.h"
#include "Vulkan/LogicalDeviceAttributes.h"
#include "Vulkan/PhysicalDeviceSelector.h"
#include "UTools/Logger/Log.h"


namespace uncanny {


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan() {
  Destroy();
}


b8 FRenderHardwareInterfaceVulkan::Create() {
  m_Destroyed = UFALSE;

  m_VolkHandler.Create();

  vulkan::FInstanceAttributes instanceAttributes{};
  instanceAttributes.Initialize();
  if (!instanceAttributes.IsVersionAvailable(VK_API_VERSION_1_3)) {
    FLog::critical("Not available vulkan version, cannot start RHI!");
    return UFALSE;
  }
  instanceAttributes.AddLayerName("VK_LAYER_KHRONOS_validation");
  instanceAttributes.AddExtensionName(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  instanceAttributes.AddExtensionName(VK_KHR_SURFACE_EXTENSION_NAME);
  instanceAttributes.AddExtensionName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  m_Instance.Create(instanceAttributes);

  m_VolkHandler.LoadInstance(m_Instance);

  m_DebugUtils.Create(m_Instance);

  {
    auto availablePhysicalDevices = m_Instance.QueryAvailablePhysicalDevices();
    VkPhysicalDevice selectedPhysicalDevice = vulkan::FPhysicalDeviceSelector().Select(availablePhysicalDevices);
    m_PhysicalDevice.Initialize(selectedPhysicalDevice);
  }

  vulkan::FLogicalDeviceAttributes logicalDeviceAttributes{};
  logicalDeviceAttributes.InitializeQueueFamilyIndexes(m_PhysicalDevice.GetAttributes().GetQueueFamilyProperties(),
                                                       m_Instance,
                                                       m_PhysicalDevice);



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
