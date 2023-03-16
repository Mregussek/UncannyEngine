
#ifndef UNCANNYENGINE_RENDERDEVICEVULKAN_H
#define UNCANNYENGINE_RENDERDEVICEVULKAN_H


#include <vector>
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Swapchain.h"


namespace uncanny::vulkan
{


class FLogicalDevice;
class FWindowSurface;


class FRenderDevice
{
public:

  void Create(const FLogicalDevice* pLogicalDevice, const FWindowSurface* pWindowSurface);
  void Destroy();

  void PrepareFrame();
  void RenderFrame();
  void PresentFrame();
  void EndFrame();

private:

  static void RecordSwapchainCommandBuffers(std::vector<FCommandBuffer>& cmdBufs, const std::vector<VkImage>& images);


  const FLogicalDevice* m_pLogicalDevice{ nullptr };
  const FWindowSurface* m_pWindowSurface{ nullptr };
  FSwapchain m_Swapchain{};
  FCommandPool m_GraphicsCommandPool{};
  FCommandPool m_TransferCommandPool{};
  FCommandPool m_ComputeCommandPool{};
  std::vector<FCommandBuffer> m_SwapchainCommandBuffers{};
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEVULKAN_H
