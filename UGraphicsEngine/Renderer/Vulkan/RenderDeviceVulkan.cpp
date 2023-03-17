
#include "RenderDeviceVulkan.h"
#include <algorithm>
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
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

  // We want unique command buffer for every image...
  m_SwapchainCommandBuffers = m_GraphicsCommandPool.AllocatePrimaryCommandBuffers(m_Swapchain.GetBackBufferCount());
  m_RecordSwapchainCommandBuffersFunc();
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

  std::ranges::for_each(m_SwapchainCommandBuffers, [](FCommandBuffer& commandBuffer)
  {
    commandBuffer.Free();
  });
  m_SwapchainCommandBuffers.clear();

  m_GraphicsCommandPool.Destroy();
  m_TransferCommandPool.Destroy();
  m_ComputeCommandPool.Destroy();

  m_Swapchain.Destroy();

  m_Context.Destroy();

  m_Destroyed = UTRUE;
}


void FRenderDevice::SetSwapchainCommandBuffersRecordingFunc(RecordSwapchainCommandBufferFunc func)
{
  m_RecordSwapchainCommandBuffersFunc = func;
}


void FRenderDevice::WaitForNextAvailableFrame()
{
  m_Swapchain.WaitForNextImage();
}


void FRenderDevice::SubmitSwapchainCommandBuffers()
{
  u32 frameIndex = m_Swapchain.GetCurrentFrameIndex();
  VkSemaphore waitSemaphores[]{ m_Swapchain.GetImageAvailableSemaphores()[frameIndex].GetHandle() };
  VkSemaphore signalSemaphores[]{ m_Swapchain.GetPresentableImageReadySemaphores()[frameIndex].GetHandle() };
  VkCommandBuffer cmdBuf[]{ m_SwapchainCommandBuffers[frameIndex].GetHandle() };
  VkFence fence{ m_Swapchain.GetFences()[frameIndex].GetHandle() };

  m_Context.GetLogicalDevice()->GetGraphicsQueue().Submit(waitSemaphores, cmdBuf, signalSemaphores,
                                                          VK_PIPELINE_STAGE_TRANSFER_BIT, fence);
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
  m_Context.GetLogicalDevice()->Wait();
  m_Swapchain.Recreate();

  m_GraphicsCommandPool.Reset();
  m_RecordSwapchainCommandBuffersFunc();
}


void FRenderDevice::RecordClearColorImageCommands()
{
  std::vector<FCommandBuffer>& cmdBufs = m_SwapchainCommandBuffers;
  const std::vector<VkImage>& images = m_Swapchain.GetImages();

  VkImageSubresourceRange subresourceRange{
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
  };

  VkClearColorValue clearColor{ 1.0f, 0.8f, 0.4f, 0.0f };

  std::ranges::for_each(cmdBufs, [images, idx = 0, &subresourceRange, clearColor](FCommandBuffer& cmdBuf) mutable
  {
    VkImage image = images[idx];

    cmdBuf.BeginRecording();
    cmdBuf.ImageMemoryBarrier(image,
                              VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    cmdBuf.ClearColorImage(image, clearColor, subresourceRange);
    cmdBuf.ImageMemoryBarrier(image,
                              VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                              subresourceRange,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    cmdBuf.EndRecording();

    idx++;
  });
}


}
