
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H

#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include "Vulkan/VolkHandler.h"
#include "Vulkan/InstanceProperties.h"
#include "Vulkan/Instance.h"


namespace uncanny {


class FRenderHardwareInterfaceVulkan {
public:

  ~FRenderHardwareInterfaceVulkan();

  b8 Create();
  void Destroy();

private:

  vulkan::FInstanceProperties m_InstanceProperties;
  vulkan::FInstance m_Instance;
  vulkan::FVolkHandler m_VolkHandler;
  // @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
