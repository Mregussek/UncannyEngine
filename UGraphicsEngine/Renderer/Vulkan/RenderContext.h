
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


/// @brief FRenderContextAttributes is a helper struct FRenderContext creation. It full-fills all
/// necessary instance layers / extensions and device extension along with expected Vulkan API version.
struct FRenderContextAttributes
{
  std::vector<const char*> instanceLayers{};
  std::vector<const char*> instanceExtensions{};
  std::vector<const char*> deviceExtensions{};
  const IWindow* pWindow{ nullptr };
  u32 apiVersion{ UUNUSED };
};


/// @brief FRenderContext is a class describing the most important parts for initialization of Vulkan API
/// in the engine.
/// @details It creates Instance, chooses physical device, creates logical device and windows surface.
/// Also it is responsible for their destruction. FLogicalDevice class is a owner of every graphics
/// queue family and actual VkQueue objects so they are a also available.
/// All contents related to this class are stored in Context directory.
class FRenderContext
{
public:

  /// @brief Creates context
  /// @details Chooses proper Vulkan API version, adds required layers and extensions, chooses physical
  /// device and then creates logical device along with window surface.
  void Create(const FRenderContextAttributes& attributes);

  /// @brief Destroys whole FRenderContext object (closes Vulkan API)
  void Destroy();

  [[nodiscard]] const FInstance* GetInstance() const { return &m_Instance; }
  [[nodiscard]] const FPhysicalDevice* GetPhysicalDevice() const { return &m_PhysicalDevice; }
  [[nodiscard]] const FLogicalDevice* GetLogicalDevice() const { return &m_LogicalDevice; }
  [[nodiscard]] const FWindowSurface* GetWindowSurface() const { return &m_WindowSurface; }

private:

  FInstance m_Instance{};
  FEXTDebugUtils m_DebugUtils{};
  FPhysicalDevice m_PhysicalDevice{};
  FVolkHandler m_VolkHandler{};
  FLogicalDevice m_LogicalDevice{};
  FWindowSurface m_WindowSurface{};
  /// @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
