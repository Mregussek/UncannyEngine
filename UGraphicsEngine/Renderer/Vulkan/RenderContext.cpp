
#include "RenderContext.h"
#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/InstanceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/PhysicalDeviceSelector.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


void FRenderContext::Create(FRenderContextAttributes attributes, const std::shared_ptr<IWindow>& pWindow)
{
  m_VolkHandler.Initialize();

  {
    FInstanceAttributes instanceAttributes{};
    instanceAttributes.Initialize();
    if (!instanceAttributes.IsVersionAvailable(attributes.apiVersion))
    {
      AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Not available vulkan version, cannot start Renderer!");
    }
    for (auto&& name : attributes.instanceLayers)
    {
      instanceAttributes.AddLayerName(name);
    }
    for (auto&& name : attributes.instanceExtensions)
    {
      instanceAttributes.AddExtensionName(name);
    }
    m_Instance.Create(instanceAttributes);
  }

  m_VolkHandler.LoadInstance(m_Instance.GetHandle());

  if (m_Instance.GetAttributes().IsExtensionRequested(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
  {
    m_DebugUtils.Create(m_Instance.GetHandle());
  }

  {
    auto availablePhysicalDevices = m_Instance.QueryAvailablePhysicalDevices();
    VkPhysicalDevice selectedPhysicalDevice = FPhysicalDeviceSelector::Select(availablePhysicalDevices);
    m_PhysicalDevice.Initialize(selectedPhysicalDevice);
  }

  {
    FLogicalDeviceAttributes logicalDeviceAttributes{};
    logicalDeviceAttributes.InitializeQueueFamilyIndexes(m_PhysicalDevice.GetAttributes().GetQueueFamilyProperties(),
                                                         m_Instance.GetHandle(),
                                                         m_PhysicalDevice.GetHandle());
    for (auto&& name : attributes.deviceExtensions)
    {
      logicalDeviceAttributes.AddExtensionName(name, m_PhysicalDevice.GetAttributes());
    }
    logicalDeviceAttributes.InitializeDeviceFeatures(m_PhysicalDevice.GetAttributes());

    m_LogicalDevice.Create(logicalDeviceAttributes, m_PhysicalDevice.GetHandle());
  }

  m_WindowSurface.Create(pWindow.get(), m_Instance.GetHandle(), m_PhysicalDevice.GetHandle());
  if (!m_WindowSurface.IsPresentationSupported(m_LogicalDevice.GetPresentFamilyIndex()))
  {
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Surface cannot present!");
  }

  m_DeviceFactory.Initialize(&m_Instance.GetAttributes(), &(GetPhysicalDevice()->GetAttributes()), GetLogicalDevice());
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
