
#include "RenderHardwareInterfaceVulkan.h"


namespace uncanny
{


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan()
{
  Destroy();
}


void FRenderHardwareInterfaceVulkan::Create(const std::shared_ptr<IWindow>& pWindow)
{
  m_Context.Create(pWindow);
  m_Device.Create(m_Context.GetLogicalDevice(), m_Context.GetWindowSurface());
}


void FRenderHardwareInterfaceVulkan::Destroy()
{
  m_Device.Destroy();
  m_Context.Destroy();
}


void FRenderHardwareInterfaceVulkan::Update()
{
  if (m_Context.GetWindowSurface()->IsMinimized())
  {
    return;
  }
  m_Device.PrepareFrame();
  m_Device.RenderFrame();
  m_Device.PresentFrame();
  m_Device.EndFrame();
}


}
