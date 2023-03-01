
#include "RenderContextVulkan.h"
#include <volk.h>
#include "UGraphicsEngine/Renderer/Vulkan/Devices/InstanceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/LogicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/PhysicalDeviceSelector.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny {


FRenderContextVulkan::~FRenderContextVulkan() {
  Destroy();
}


void FRenderContextVulkan::Create(const std::shared_ptr<IWindow>& pWindow) {
  m_pWindow = pWindow;

  m_VolkHandler.Create();

  {
    vulkan::FInstanceAttributes instanceAttributes{};
    instanceAttributes.Initialize();
    if (!instanceAttributes.IsVersionAvailable(VK_API_VERSION_1_3)) {
      vulkan::AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Not available vulkan version, cannot start Renderer!");
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

  m_WindowSurface.Create(m_pWindow.get(), m_Instance.GetHandle(), m_PhysicalDevice.GetHandle());
  if (!m_WindowSurface.IsPresentationSupported(m_LogicalDevice.GetPresentQueueFamilyIndex())) {
    vulkan::AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Surface cannot present!");
  }
  m_WindowSurface.UpdateCapabilities();
  m_WindowSurface.UpdateFormats();
  m_WindowSurface.UpdatePresentModes();
}


void FRenderContextVulkan::Destroy() {
  if (m_Destroyed) {
    return;
  }
  if (m_LogicalDevice.IsValid()) {
    m_LogicalDevice.Wait();
  }

  m_WindowSurface.Destroy(m_Instance.GetHandle());
  m_LogicalDevice.Destroy();
  m_DebugUtils.Destroy(m_Instance.GetHandle());
  m_Instance.Destroy();
  m_VolkHandler.Destroy();
  m_Destroyed = UTRUE;
}


}
