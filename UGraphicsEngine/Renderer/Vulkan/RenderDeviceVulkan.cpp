
#include "RenderDeviceVulkan.h"
#include <algorithm>
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"


namespace uncanny::vulkan
{


void FRenderDevice::Create(const vulkan::FLogicalDevice* pLogicalDevice, const vulkan::FWindowSurface* pWindowSurface)
{
  m_pLogicalDevice = pLogicalDevice;
  m_pWindowSurface = pWindowSurface;

  u32 backBufferCount{ 2 };
  m_Swapchain.Create(backBufferCount, m_pLogicalDevice->GetHandle(), &m_pLogicalDevice->GetPresentQueue(),
                     m_pWindowSurface);
  m_Swapchain.Recreate();

  m_GraphicsCommandPool.Create(m_pLogicalDevice->GetGraphicsFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_TransferCommandPool.Create(m_pLogicalDevice->GetTransferFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_ComputeCommandPool.Create(m_pLogicalDevice->GetComputeFamilyIndex(), m_pLogicalDevice->GetHandle());

  m_RenderCommandBuffers = m_GraphicsCommandPool.AllocatePrimaryCommandBuffers(m_Swapchain.GetBackBufferCount());
}


void FRenderDevice::Destroy()
{
  if (m_Destroyed)
  {
    return;
  }
  if (m_pLogicalDevice->IsValid())
  {
    m_pLogicalDevice->Wait();
  }

  std::ranges::for_each(m_RenderCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
  {
    commandBuffer.Free();
  });
  m_RenderCommandBuffers.clear();

  m_GraphicsCommandPool.Destroy();
  m_TransferCommandPool.Destroy();
  m_ComputeCommandPool.Destroy();

  m_Swapchain.Destroy();

  m_Destroyed = UTRUE;
}


void FRenderDevice::PrepareFrame()
{
  m_Swapchain.WaitForNextImage();

  u32 frameIndex = m_Swapchain.GetCurrentFrameIndex();
  VkImage image = m_Swapchain.GetImages()[frameIndex];

  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkClearColorValue clearColor = {{ 1.0f, 0.8f, 0.4f, 0.0f }};

  m_RenderCommandBuffers[frameIndex].BeginRecording();

  m_RenderCommandBuffers[frameIndex].ImageMemoryBarrierToStartTransfer(image);
  vkCmdClearColorImage(m_RenderCommandBuffers[frameIndex].GetHandle(),
                       image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       &clearColor,
                       1, &imageSubresourceRange);
  m_RenderCommandBuffers[frameIndex].ImageMemoryBarrierToFinishTransferAndStartPresentation(image);

  m_RenderCommandBuffers[frameIndex].EndRecording();
}


void FRenderDevice::RenderFrame()
{
  VkPipelineStageFlags waitDstStageMask{ VK_PIPELINE_STAGE_TRANSFER_BIT };
  VkSemaphore waitSemaphores[]{ m_Swapchain.GetImageAvailableSemaphore() };
  VkSemaphore signalSemaphores[]{ m_Swapchain.GetPresentableImageReadySemaphore() };
  VkCommandBuffer commandBuffers[]{ m_RenderCommandBuffers[m_Swapchain.GetCurrentFrameIndex()].GetHandle() };

  VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  submitInfo.pNext = nullptr;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = &waitDstStageMask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = commandBuffers;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VkResult result = vkQueueSubmit(m_pLogicalDevice->GetGraphicsQueue().GetHandle(), 1, &submitInfo,
                                  m_Swapchain.GetFence());
  AssertVkAndThrow(result);
}


void FRenderDevice::PresentFrame()
{
  m_Swapchain.Present();
}


void FRenderDevice::EndFrame()
{
  if (m_Swapchain.IsOutOfDate())
  {
    m_pLogicalDevice->Wait();
    m_Swapchain.Recreate();
  }
}


}
