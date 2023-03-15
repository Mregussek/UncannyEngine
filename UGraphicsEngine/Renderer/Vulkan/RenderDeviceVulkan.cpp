
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

  m_GraphicsCommandPool.Create(m_pLogicalDevice->GetGraphicsFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_TransferCommandPool.Create(m_pLogicalDevice->GetTransferFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_ComputeCommandPool.Create(m_pLogicalDevice->GetComputeFamilyIndex(), m_pLogicalDevice->GetHandle());

  // We want unique command buffer for every image...
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

  VkImageSubresourceRange imageSubresourceRange{
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0,
    .levelCount = 1,
    .baseArrayLayer = 0,
    .layerCount = 1
  };

  VkClearColorValue clearColor{ 1.0f, 0.8f, 0.4f, 0.0f };

  { // Recording command buffer...
    u32 frameIndex = m_Swapchain.GetCurrentFrameIndex();
    FCommandBuffer& renderCommandBuffer{ m_RenderCommandBuffers[frameIndex] };
    VkImage image = m_Swapchain.GetImages()[frameIndex];

    renderCommandBuffer.BeginRecording();
    renderCommandBuffer.ImageMemoryBarrierToStartTransfer(image);
    vkCmdClearColorImage(renderCommandBuffer.GetHandle(),
                         image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         &clearColor,
                         1, &imageSubresourceRange);
    renderCommandBuffer.ImageMemoryBarrierToFinishTransferAndStartPresentation(image);
    renderCommandBuffer.EndRecording();
  }

  VkPipelineStageFlags waitDstStageMask{ VK_PIPELINE_STAGE_TRANSFER_BIT };
  VkSemaphore waitSemaphores[]{ m_Swapchain.GetImageAvailableSemaphore() };
  VkSemaphore signalSemaphores[]{ m_Swapchain.GetPresentableImageReadySemaphore() };
  VkCommandBuffer commandBuffers[]{ m_RenderCommandBuffers[m_Swapchain.GetCurrentFrameIndex()].GetHandle() };

  VkSubmitInfo submitInfo{
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = waitSemaphores,
    .pWaitDstStageMask = &waitDstStageMask,
    .commandBufferCount = 1,
    .pCommandBuffers = commandBuffers,
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = signalSemaphores
  };

  VkResult result = vkQueueSubmit(m_pLogicalDevice->GetGraphicsQueue().GetHandle(), 1, &submitInfo,
                                  m_Swapchain.GetFence());
  AssertVkAndThrow(result);
}


void FRenderDevice::RenderFrame()
{

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
