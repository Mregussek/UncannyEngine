
#ifndef UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
#define UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H

#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include "UGraphicsEngine/Renderer/Vulkan/Devices/VolkHandler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/Instance.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/EXTDebugUtils.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/PhysicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"


namespace uncanny {


class FRenderHardwareInterfaceVulkan {
public:

  ~FRenderHardwareInterfaceVulkan();

  void Create();
  void Destroy();

private:

  vulkan::FInstance m_Instance{};
  vulkan::FEXTDebugUtils m_DebugUtils{};
  vulkan::FPhysicalDevice m_PhysicalDevice{};
  vulkan::FLogicalDevice m_LogicalDevice{};
  vulkan::FVolkHandler m_VolkHandler{};
  vulkan::FCommandPool m_GraphicsCommandPool{ VK_NULL_HANDLE, VK_NULL_HANDLE };
  vulkan::FCommandPool m_TransferCommandPool{ VK_NULL_HANDLE, VK_NULL_HANDLE };
  vulkan::FCommandPool m_ComputeCommandPool{ VK_NULL_HANDLE, VK_NULL_HANDLE };
  std::vector<vulkan::FCommandBuffer> m_RenderCommandBuffers{};
  std::vector<vulkan::FCommandBuffer> m_TransferCommandBuffers{};
  // @brief boolean guard for Destroy() method in destructor
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERHARDWAREINTERFACEVULKAN_H
