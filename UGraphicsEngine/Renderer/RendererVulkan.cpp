
#include "RendererVulkan.h"
#include <algorithm>
#include <utility>


namespace uncanny {


FRendererVulkan::~FRendererVulkan() {
  Destroy();
}


void FRendererVulkan::Create(const vulkan::FLogicalDevice* pLogicalDevice) {
  m_pLogicalDevice = pLogicalDevice;

  vulkan::FLogicalDeviceFactory logicalDeviceFactory = m_pLogicalDevice->GetFactory();
  m_GraphicsCommandPool = logicalDeviceFactory.CreateCommandPool(m_pLogicalDevice->GetGraphicsQueueFamilyIndex());
  m_TransferCommandPool = logicalDeviceFactory.CreateCommandPool(m_pLogicalDevice->GetTransferQueueFamilyIndex());
  m_ComputeCommandPool = logicalDeviceFactory.CreateCommandPool(m_pLogicalDevice->GetComputeQueueFamilyIndex());

  m_RenderCommandBuffers = m_GraphicsCommandPool.GetFactory().AllocatePrimaryCommandBuffers(2);
  m_TransferCommandBuffers = m_TransferCommandPool.GetFactory().AllocatePrimaryCommandBuffers(2);
}


void FRendererVulkan::Destroy() {
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
