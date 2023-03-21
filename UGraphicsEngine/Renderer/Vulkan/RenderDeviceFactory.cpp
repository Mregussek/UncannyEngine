
#include "RenderDeviceFactory.h"
#include "Context/PhysicalDevice.h"
#include "Context/LogicalDevice.h"
#include <algorithm>


namespace uncanny::vulkan
{


FBuffer FRenderDeviceFactory::CreateBuffer() const
{
  return FBuffer{ &m_pPhysicalDevice->GetAttributes(), m_pLogicalDevice->GetHandle() };
}


std::vector<FImage> FRenderDeviceFactory::CreateImages(u32 count) const
{
  std::vector<FImage> images{};
  images.reserve(count);
  for(u32 i = 0; i < count; i++)
  {
    images.push_back({&m_pPhysicalDevice->GetAttributes(), m_pLogicalDevice->GetHandle()});
  }
  return images;
}


std::vector<FSemaphore> FRenderDeviceFactory::CreateSemaphores(u32 count) const
{
  std::vector<FSemaphore> semaphores(count);
  std::ranges::for_each(semaphores, [this](FSemaphore& semaphore)
  {
    semaphore.Create(m_pLogicalDevice->GetHandle());
  });
  return semaphores;
}


void FRenderDeviceFactory::Initialize(const FPhysicalDevice* pPhysicalDevice, const FLogicalDevice* pLogicalDevice)
{
  m_pPhysicalDevice = pPhysicalDevice;
  m_pLogicalDevice = pLogicalDevice;
}


}
