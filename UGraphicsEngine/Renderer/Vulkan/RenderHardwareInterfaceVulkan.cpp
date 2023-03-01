
#include "RenderHardwareInterfaceVulkan.h"


namespace uncanny {


FRenderHardwareInterfaceVulkan::~FRenderHardwareInterfaceVulkan() {
  Destroy();
}


void FRenderHardwareInterfaceVulkan::Create(const std::shared_ptr<IWindow>& pWindow) {
  m_Context.Create(pWindow);
  m_Device.Create(m_Context.GetLogicalDevice(), m_Context.GetWindowSurface());
}


void FRenderHardwareInterfaceVulkan::Destroy() {
  m_Device.Destroy();
  m_Context.Destroy();
}


}
