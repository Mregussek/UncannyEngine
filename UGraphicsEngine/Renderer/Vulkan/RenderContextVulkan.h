
#ifndef UNCANNYENGINE_RENDERCONTEXTVULKAN_H
#define UNCANNYENGINE_RENDERCONTEXTVULKAN_H

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


namespace uncanny::vulkan {


class FRenderContext {
public:

  void Create(const std::shared_ptr<IWindow>& pWindow);
  void Destroy();

  [[nodiscard]] const FLogicalDevice* GetLogicalDevice() const { return &m_LogicalDevice; }
  [[nodiscard]] const FWindowSurface* GetWindowSurface() const { return &m_WindowSurface; }

private:

  FInstance m_Instance{};
  FEXTDebugUtils m_DebugUtils{};
  FPhysicalDevice m_PhysicalDevice{};
  FVolkHandler m_VolkHandler{};
  FLogicalDevice m_LogicalDevice{};
  FWindowSurface m_WindowSurface{};
  std::shared_ptr<IWindow> m_pWindow{};
  // @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
