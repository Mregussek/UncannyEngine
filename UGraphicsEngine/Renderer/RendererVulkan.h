
#ifndef UNCANNYENGINE_RENDERERVULKAN_H
#define UNCANNYENGINE_RENDERERVULKAN_H


#include <vector>
#include "UGraphicsEngine/Renderer/Vulkan/Devices/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/WindowSurface.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"


namespace uncanny {


class FRendererVulkan {
public:

  ~FRendererVulkan();

  void Create(const vulkan::FLogicalDevice* pLogicalDevice, const vulkan::FWindowSurface* pWindowSurface);
  void Destroy();

private:

  const vulkan::FLogicalDevice* m_pLogicalDevice{ nullptr };
  const vulkan::FWindowSurface* m_pWindowSurface{ nullptr };
  vulkan::FCommandPool m_GraphicsCommandPool{};
  vulkan::FCommandPool m_TransferCommandPool{};
  vulkan::FCommandPool m_ComputeCommandPool{};
  std::vector<vulkan::FCommandBuffer> m_RenderCommandBuffers{};
  std::vector<vulkan::FCommandBuffer> m_TransferCommandBuffers{};
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERERVULKAN_H
