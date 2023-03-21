
#include "RenderContext.h"
#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/InstanceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/PhysicalDeviceSelector.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


void FRenderContext::Create(const std::shared_ptr<IWindow>& pWindow)
{
  m_VolkHandler.Initialize();

  {
    vulkan::FInstanceAttributes instanceAttributes{};
    instanceAttributes.Initialize();
    if (!instanceAttributes.IsVersionAvailable(VK_API_VERSION_1_3))
    {
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
    VkPhysicalDevice selectedPhysicalDevice = vulkan::FPhysicalDeviceSelector::Select(availablePhysicalDevices);
    m_PhysicalDevice.Initialize(selectedPhysicalDevice);
  }

  {
    vulkan::FLogicalDeviceAttributes logicalDeviceAttributes{};
    logicalDeviceAttributes.InitializeQueueFamilyIndexes(m_PhysicalDevice.GetAttributes().GetQueueFamilyProperties(),
                                                         m_Instance.GetHandle(),
                                                         m_PhysicalDevice.GetHandle());
    logicalDeviceAttributes.AddExtensionName(VK_KHR_SWAPCHAIN_EXTENSION_NAME, m_PhysicalDevice.GetAttributes());
    logicalDeviceAttributes.InitializeDeviceFeatures(m_PhysicalDevice.GetAttributes().GetDeviceFeatures());

    m_LogicalDevice.Create(logicalDeviceAttributes, m_PhysicalDevice.GetHandle());
  }

  m_WindowSurface.Create(pWindow.get(), m_Instance.GetHandle(), m_PhysicalDevice.GetHandle());
  if (!m_WindowSurface.IsPresentationSupported(m_LogicalDevice.GetPresentFamilyIndex()))
  {
    vulkan::AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Surface cannot present!");
  }
}


void FRenderContext::Destroy()
{
  if (m_Destroyed)
  {
    return;
  }
  if (m_LogicalDevice.IsValid())
  {
    m_LogicalDevice.WaitIdle();
  }

  m_WindowSurface.Destroy(m_Instance.GetHandle());
  m_LogicalDevice.Destroy();
  m_DebugUtils.Destroy(m_Instance.GetHandle());
  m_Instance.Destroy();
  m_Destroyed = UTRUE;
}


}
