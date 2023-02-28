
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H

#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include <memory>
#include "UGraphicsEngine/Renderer/Vulkan/Devices/VolkHandler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/Instance.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/EXTDebugUtils.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/PhysicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/WindowSurface.h"
#include "UTools/Window/IWindow.h"


namespace uncanny {


class FRenderHardwareInterfaceVulkan {
public:

  ~FRenderHardwareInterfaceVulkan();

  void Create(const std::shared_ptr<IWindow>& pWindow);
  void Destroy();

  [[nodiscard]] const vulkan::FLogicalDevice& GetLogicalDevice() const { return m_LogicalDevice; }

private:

  vulkan::FInstance m_Instance{};
  vulkan::FEXTDebugUtils m_DebugUtils{};
  vulkan::FPhysicalDevice m_PhysicalDevice{};
  vulkan::FLogicalDevice m_LogicalDevice{};
  vulkan::FVolkHandler m_VolkHandler{};
  std::shared_ptr<IWindow> m_pWindow{};
  // @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
