
#ifndef UNCANNYENGINE_RENDERDEVICEVULKAN_H
#define UNCANNYENGINE_RENDERDEVICEVULKAN_H


#include <vector>
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"


namespace uncanny::vulkan {


class FLogicalDevice;
class FWindowSurface;


class FRenderDevice {
public:

  void Create(const FLogicalDevice* pLogicalDevice, const FWindowSurface* pWindowSurface);
  void Destroy();

private:

  const FLogicalDevice* m_pLogicalDevice{ nullptr };
  const FWindowSurface* m_pWindowSurface{ nullptr };
  FCommandPool m_GraphicsCommandPool{};
  FCommandPool m_TransferCommandPool{};
  FCommandPool m_ComputeCommandPool{};
  std::vector<FCommandBuffer> m_RenderCommandBuffers{};
  std::vector<FCommandBuffer> m_TransferCommandBuffers{};
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEVULKAN_H
