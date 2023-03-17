
#ifndef UNCANNYENGINE_RENDERDEVICEVULKAN_H
#define UNCANNYENGINE_RENDERDEVICEVULKAN_H


#include <vector>
#include <functional>
#include "RenderContextVulkan.h"
#include "Commands/CommandPool.h"
#include "Commands/CommandBuffer.h"
#include "Device/Swapchain.h"


namespace uncanny::vulkan
{


class FLogicalDevice;
class FWindowSurface;


typedef std::function<void()> RecordSwapchainCommandBufferFunc;


class FRenderDevice
{
public:

  void Create(const std::shared_ptr<IWindow>& pWindow, u32 backBufferCount);
  void Destroy();

  void SetSwapchainCommandBuffersRecordingFunc(RecordSwapchainCommandBufferFunc func);

  void WaitForNextAvailableFrame();
  void SubmitSwapchainCommandBuffers();
  void PresentFrame();

  [[nodiscard]] b8 IsOutOfDate() const;
  void RecreateRenderingResources();

  void RecordClearColorImageCommands();

private:

  FRenderContext m_Context{};
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
