
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H


#include "RenderContextVulkan.h"
#include "RenderDeviceVulkan.h"


namespace uncanny {


class IWindow;


class FRenderHardwareInterfaceVulkan {
public:

  void Create(const std::shared_ptr<IWindow>& pWindow);
  void Destroy();

private:

  vulkan::FRenderContext m_Context{};
  vulkan::FRenderDevice m_Device{};

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
