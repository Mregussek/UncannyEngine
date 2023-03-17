
#ifndef UNCANNYENGINE_RENDERCONTEXT_H
#define UNCANNYENGINE_RENDERCONTEXT_H

#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include <memory>
#include "UGraphicsEngine/Renderer/Vulkan/Context/VolkHandler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Instance.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/EXTDebugUtils.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/PhysicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"


namespace uncanny::vulkan
{


class FRenderContext
{
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


#endif //UNCANNYENGINE_RENDERCONTEXT_H
