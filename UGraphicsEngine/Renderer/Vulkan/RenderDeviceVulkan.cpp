
#include "RenderDeviceVulkan.h"
#include <algorithm>
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"


namespace uncanny::vulkan
{


void FRenderDevice::Create(const vulkan::FLogicalDevice* pLogicalDevice, const vulkan::FWindowSurface* pWindowSurface)
{
  m_pLogicalDevice = pLogicalDevice;
  m_pWindowSurface = pWindowSurface;

  u32 backBufferCount{ 2 };
  m_Swapchain.Create(backBufferCount, m_pLogicalDevice->GetHandle(), m_pWindowSurface);
  m_Swapchain.Recreate();

  m_GraphicsCommandPool.Create(m_pLogicalDevice->GetGraphicsFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_TransferCommandPool.Create(m_pLogicalDevice->GetTransferFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_ComputeCommandPool.Create(m_pLogicalDevice->GetComputeFamilyIndex(), m_pLogicalDevice->GetHandle());

  m_RenderCommandBuffers = m_GraphicsCommandPool.AllocatePrimaryCommandBuffers(2);
  m_TransferCommandBuffers = m_TransferCommandPool.AllocatePrimaryCommandBuffers(2);
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
  std::ranges::for_each(m_TransferCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
  {
    commandBuffer.Free();
  });
  m_TransferCommandBuffers.clear();

  m_GraphicsCommandPool.Destroy();
  m_TransferCommandPool.Destroy();
  m_ComputeCommandPool.Destroy();

  m_Swapchain.Destroy();

  m_Destroyed = UTRUE;
}


}
