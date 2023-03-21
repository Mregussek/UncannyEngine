
#ifndef UNCANNYENGINE_RENDERDEVICE_H
#define UNCANNYENGINE_RENDERDEVICE_H


#include <utility>
#include <vector>
#include <functional>
#include "RenderContext.h"
#include "RenderDeviceFactory.h"
#include "Commands/CommandPool.h"
#include "Commands/CommandBuffer.h"
#include "Device/Swapchain.h"


namespace uncanny::vulkan
{


class FLogicalDevice;
class FWindowSurface;


typedef std::function<void()> FRenderDeviceCallbackFunc;


class FRenderDevice
{
public:

  void Create(const std::shared_ptr<IWindow>& pWindow, u32 backBufferCount);
  void Destroy();

  void SetRecreateRenderingResourcesCallback(FRenderDeviceCallbackFunc func);

  void WaitIdle() const;

  void WaitForNextAvailableFrame();
  void PresentFrame();

  [[nodiscard]] b8 IsOutOfDate() const;
  void RecreateRenderingResources();

  [[nodiscard]] const FRenderDeviceFactory& GetFactory() const { return m_Factory; }
  [[nodiscard]] const FSwapchain& GetSwapchain() const { return m_Swapchain; }
  [[nodiscard]] const FCommandPool& GetGraphicsCommandPool() const { return m_GraphicsCommandPool; }
  [[nodiscard]] const FCommandPool& GetTransferCommandPool() const { return m_TransferCommandPool; }
  [[nodiscard]] const FCommandPool& GetComputeCommandPool() const { return m_ComputeCommandPool; }
  [[nodiscard]] const FQueue& GetGraphicsQueue() const { return m_Context.GetLogicalDevice()->GetGraphicsQueue(); }
  [[nodiscard]] const FQueue& GetTransferQueue() const { return m_Context.GetLogicalDevice()->GetTransferQueue(); }
  [[nodiscard]] const FQueue& GetComputeQueue() const { return m_Context.GetLogicalDevice()->GetComputeQueue(); }

private:

  FRenderContext m_Context{};
  FSwapchain m_Swapchain{};
  FCommandPool m_GraphicsCommandPool{};
  FCommandPool m_TransferCommandPool{};
  FCommandPool m_ComputeCommandPool{};
  FRenderDeviceFactory m_Factory{};
  FRenderDeviceCallbackFunc m_RecreateRenderResourcesCallback{};
  b8 m_Destroyed{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERDEVICE_H
