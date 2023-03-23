
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
#include "UGraphicsEngine/Renderer/Vulkan/RenderDeviceFactory.h"


namespace uncanny::vulkan
{


/// @brief FRenderContext is a class describing the most important parts for initialization of Vulkan API
/// in the engine.
/// @details It creates Instance, chooses physical device, creates logical device and windows surface.
/// Also it is responsible for their destruction. It must not be exposed to the end user. Only FRenderDevice
/// should use it only for creation / destruction and handles retrieval.
class FRenderContext
{
public:

  /// @brief Creates context
  /// @details Chooses proper Vulkan API version, adds required layers and extensions, chooses physical
  /// device and then creates logical device along with window surface.
  /// @param pWindow pointer to window interface (needed for window surface creation)
  void Create(const std::shared_ptr<IWindow>& pWindow);

  /// @brief Destroys whole FRenderContext object (closes Vulkan API)
  void Destroy();

  [[nodiscard]] const FPhysicalDevice* GetPhysicalDevice() const { return &m_PhysicalDevice; }
  [[nodiscard]] const FLogicalDevice* GetLogicalDevice() const { return &m_LogicalDevice; }
  [[nodiscard]] const FWindowSurface* GetWindowSurface() const { return &m_WindowSurface; }
  [[nodiscard]] const FRenderDeviceFactory& GetFactory() const { return m_DeviceFactory; }

private:

  FInstance m_Instance{};
  FEXTDebugUtils m_DebugUtils{};
  FPhysicalDevice m_PhysicalDevice{};
  FVolkHandler m_VolkHandler{};
  FLogicalDevice m_LogicalDevice{};
  FWindowSurface m_WindowSurface{};
  FRenderDeviceFactory m_DeviceFactory{};
  /// @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
