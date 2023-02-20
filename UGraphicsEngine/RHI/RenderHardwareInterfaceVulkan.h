
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H

#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include "Vulkan/VolkHandler.h"
#include "Vulkan/InstanceAttributes.h"
#include "Vulkan/Instance.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/EXTDebugUtils.h"


namespace uncanny {


class FRenderHardwareInterfaceVulkan {
public:

  ~FRenderHardwareInterfaceVulkan();

  b8 Create();
  void Destroy();

private:

  vulkan::FInstanceAttributes m_InstanceAttributes{};
  vulkan::FInstance m_Instance{};
  vulkan::FEXTDebugUtils m_DebugUtils{};
  vulkan::FPhysicalDevice m_PhysicalDevice{};
  vulkan::FVolkHandler m_VolkHandler{};
  // @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
