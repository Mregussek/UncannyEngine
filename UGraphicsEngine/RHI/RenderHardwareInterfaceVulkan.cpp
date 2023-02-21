
#include "RenderHardwareInterfaceVulkan.h"
#include <volk.h>
#include "UGraphicsEngine/RHI/Vulkan/Devices/InstanceAttributes.h"
#include "UGraphicsEngine/RHI/Vulkan/Devices/LogicalDeviceAttributes.h"
#include "UGraphicsEngine/RHI/Vulkan/Devices/PhysicalDeviceSelector.h"
#include "UTools/Logger/Log.h"


namespace uncanny {


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan() {
  Destroy();
}


b8 FRenderHardwareInterfaceVulkan::Create() {
  m_Destroyed = UFALSE;

  m_VolkHandler.Create();

  {
    vulkan::FInstanceAttributes instanceAttributes{};
    instanceAttributes.Initialize();
    if (!instanceAttributes.IsVersionAvailable(VK_API_VERSION_1_3)) {
      UCRITICAL("Not available vulkan version, cannot start RHI!");
      return UFALSE;
    }
    instanceAttributes.AddLayerName("VK_LAYER_KHRONOS_validation");
    instanceAttributes.AddExtensionName(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    instanceAttributes.AddExtensionName(VK_KHR_SURFACE_EXTENSION_NAME);
    instanceAttributes.AddExtensionName(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    m_Instance.Create(instanceAttributes);
  }

  m_VolkHandler.LoadInstance(m_Instance.GetHandle());
  m_DebugUtils.Create(m_Instance.GetHandle());

  {
    auto availablePhysicalDevices = m_Instance.QueryAvailablePhysicalDevices();
    VkPhysicalDevice selectedPhysicalDevice = vulkan::FPhysicalDeviceSelector().Select(availablePhysicalDevices);
    m_PhysicalDevice.Initialize(selectedPhysicalDevice);
  }

  {
    vulkan::FLogicalDeviceAttributes logicalDeviceAttributes{};
    logicalDeviceAttributes.InitializeQueueFamilyIndexes(m_PhysicalDevice.GetAttributes().GetQueueFamilyProperties(),
                                                         m_Instance.GetHandle(),
                                                         m_PhysicalDevice.GetHandle());
    logicalDeviceAttributes.AddExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME, m_PhysicalDevice.GetAttributes());

    m_LogicalDevice.Create(logicalDeviceAttributes, m_PhysicalDevice.GetHandle());
  }

  vulkan::FLogicalDeviceFactory logicalDeviceFactory = m_LogicalDevice.GetFactory();
  m_GraphicsCommandPool = logicalDeviceFactory.CreateCommandPool(m_LogicalDevice.GetGraphicsQueueFamilyIndex());
  m_PresentCommandPool = logicalDeviceFactory.CreateCommandPool(m_LogicalDevice.GetPresentQueueFamilyIndex());
  m_TransferCommandPool = logicalDeviceFactory.CreateCommandPool(m_LogicalDevice.GetTransferQueueFamilyIndex());
  m_ComputeCommandPool = logicalDeviceFactory.CreateCommandPool(m_LogicalDevice.GetComputeQueueFamilyIndex());

  return UTRUE;
}


void FRenderHardwareInterfaceVulkan::Destroy() {
  if (m_Destroyed) {
    return;
  }
  m_LogicalDevice.Wait();

  m_GraphicsCommandPool.Destroy();
  m_PresentCommandPool.Destroy();
  m_TransferCommandPool.Destroy();
  m_ComputeCommandPool.Destroy();
  m_LogicalDevice.Destroy();
  m_DebugUtils.Destroy(m_Instance.GetHandle());
  m_Instance.Destroy();
  m_VolkHandler.Destroy();
  m_Destroyed = UTRUE;
}


}
