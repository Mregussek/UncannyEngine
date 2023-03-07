
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H


#include "RenderContextVulkan.h"
#include "RenderDeviceVulkan.h"


namespace uncanny
{


class IWindow;


/*
 * @brief RHI must be responsible for lifetime for RenderContext and RenderDevice
 */
class FRenderHardwareInterfaceVulkan
{
public:

  ~FRenderHardwareInterfaceVulkan();

  void Create(const std::shared_ptr<IWindow>& pWindow);
  void Destroy();

  void Update();

private:

  vulkan::FRenderContext m_Context{};
  vulkan::FRenderDevice m_Device{};

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
