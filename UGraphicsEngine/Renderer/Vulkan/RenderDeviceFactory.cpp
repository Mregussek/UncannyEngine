
#include "RenderDeviceFactory.h"
#include "Context/PhysicalDevice.h"
#include "Context/LogicalDevice.h"


namespace uncanny::vulkan
{


FBuffer FRenderDeviceFactory::CreateBuffer() const
{
  return FBuffer{ &m_pPhysicalDevice->GetAttributes(), m_pLogicalDevice->GetHandle() };
}


void FRenderDeviceFactory::Initialize(const FPhysicalDevice* pPhysicalDevice, const FLogicalDevice* pLogicalDevice)
{
  m_pPhysicalDevice = pPhysicalDevice;
  m_pLogicalDevice = pLogicalDevice;
}


}
