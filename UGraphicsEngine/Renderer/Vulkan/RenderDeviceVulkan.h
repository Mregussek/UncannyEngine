
#ifndef UNCANNYENGINE_RENDERDEVICEVULKAN_H
#define UNCANNYENGINE_RENDERDEVICEVULKAN_H


#include <vector>
#include <functional>
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Swapchain.h"


namespace uncanny::vulkan
{


class FLogicalDevice;
class FWindowSurface;


typedef std::function<void()> RecordSwapchainCommandBufferFunc;


class FRenderDevice
{
public:

  void Create(const FLogicalDevice* pLogicalDevice, const FWindowSurface* pWindowSurface, u32 backBufferCount);
  void Destroy();

  void SetSwapchainCommandBuffersRecordingFunc(RecordSwapchainCommandBufferFunc func);

  void WaitForNextAvailableFrame();
  void RenderFrame();
  void PresentFrame();

  [[nodiscard]] b8 IsOutOfDate() const;
  void RecreateRenderingResources();

  void RecordClearColorImageCommands();

private:

  const FLogicalDevice* m_pLogicalDevice{ nullptr };
  const FWindowSurface* m_pWindowSurface{ nullptr };
  FSwapchain m_Swapchain{};
  FCommandPool m_GraphicsCommandPool{};
  FCommandPool m_TransferCommandPool{};
  FCommandPool m_ComputeCommandPool{};
  std::vector<FCommandBuffer> m_SwapchainCommandBuffers{};
  RecordSwapchainCommandBufferFunc m_RecordSwapchainCommandBuffersFunc{};
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEVULKAN_H
