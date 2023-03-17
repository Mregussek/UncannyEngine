
#include "RenderDevice.h"
#include <algorithm>
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"
#include "UTools/Window/IWindow.h"


namespace uncanny::vulkan
{


void FRenderDevice::Create(const std::shared_ptr<IWindow>& pWindow, u32 backBufferCount)
{
  m_Context.Create(pWindow);
  const FLogicalDevice* pLogicalDevice = m_Context.GetLogicalDevice();
  const FWindowSurface* pWindowSurface = m_Context.GetWindowSurface();

  m_Swapchain.Create(backBufferCount, pLogicalDevice->GetHandle(), &pLogicalDevice->GetPresentQueue(),
                     pWindowSurface);

  m_GraphicsCommandPool.Create(pLogicalDevice->GetGraphicsFamilyIndex(), pLogicalDevice->GetHandle());
  m_TransferCommandPool.Create(pLogicalDevice->GetTransferFamilyIndex(), pLogicalDevice->GetHandle());
  m_ComputeCommandPool.Create(pLogicalDevice->GetComputeFamilyIndex(), pLogicalDevice->GetHandle());
}


void FRenderDevice::Destroy()
{
  if (m_Destroyed)
  {
    return;
  }
  if (m_Context.GetLogicalDevice()->IsValid())
  {
    m_Context.GetLogicalDevice()->Wait();
  }

  m_GraphicsCommandPool.Destroy();
  m_TransferCommandPool.Destroy();
  m_ComputeCommandPool.Destroy();

  m_Swapchain.Destroy();

  m_Context.Destroy();

  m_Destroyed = UTRUE;
}


void FRenderDevice::SetRecordingCommandsFunc(FRecordCommandsFunc func)
{
  m_RecordCommandsFunc = std::move(func);
  m_RecordCommandsFunc();
}


void FRenderDevice::WaitIdle() const
{
  m_Context.GetLogicalDevice()->Wait();
}


void FRenderDevice::WaitForNextAvailableFrame()
{
  m_Swapchain.WaitForNextImage();
}


void FRenderDevice::PresentFrame()
{
  m_Swapchain.Present();
}


b8 FRenderDevice::IsOutOfDate() const
{
  return m_Swapchain.IsOutOfDate();
}


void FRenderDevice::RecreateRenderingResources()
{
  WaitIdle();
  m_Swapchain.Recreate();

  m_GraphicsCommandPool.Reset();
  m_RecordCommandsFunc();
}


}