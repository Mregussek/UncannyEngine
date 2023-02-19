
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H

#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include "Vulkan/VolkHandler.h"
#include "Vulkan/InstanceProperties.h"


namespace uncanny {


class FRenderHardwareInterfaceVulkan {
public:

  void Create();
  void Destroy();

private:

  vulkan::FInstanceProperties m_InstanceProperties;
  vulkan::FVolkHandler m_VolkHandler;

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
