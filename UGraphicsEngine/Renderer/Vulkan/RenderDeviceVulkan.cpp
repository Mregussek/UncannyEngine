
#include "RenderDeviceVulkan.h"
#include <algorithm>


namespace uncanny::vulkan {


FRenderDevice::~FRenderDevice() {
  Destroy();
}


void FRenderDevice::Create(const vulkan::FLogicalDevice* pLogicalDevice, const vulkan::FWindowSurface* pWindowSurface) {
  m_pLogicalDevice = pLogicalDevice;
  m_pWindowSurface = pWindowSurface;

  m_GraphicsCommandPool.Create(m_pLogicalDevice->GetGraphicsQueueFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_TransferCommandPool.Create(m_pLogicalDevice->GetTransferQueueFamilyIndex(), m_pLogicalDevice->GetHandle());
  m_ComputeCommandPool.Create(m_pLogicalDevice->GetComputeQueueFamilyIndex(), m_pLogicalDevice->GetHandle());

  m_RenderCommandBuffers = m_GraphicsCommandPool.AllocatePrimaryCommandBuffers(2);
  m_TransferCommandBuffers = m_TransferCommandPool.AllocatePrimaryCommandBuffers(2);
}


void FRenderDevice::Destroy() {
  if (m_Destroyed) {
    return;
  }
  if (m_pLogicalDevice->IsValid()) {
    m_pLogicalDevice->Wait();
  }

  std::ranges::for_each(m_RenderCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer){
    commandBuffer.Free();
  });
  m_RenderCommandBuffers.clear();
  std::ranges::for_each(m_TransferCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer){
    commandBuffer.Free();
  });
  m_TransferCommandBuffers.clear();

  m_GraphicsCommandPool.Destroy();
  m_TransferCommandPool.Destroy();
  m_ComputeCommandPool.Destroy();

  m_Destroyed = UTRUE;
}


}
