
#include "RenderDeviceFactory.h"
#include "Context/InstanceAttributes.h"
#include "Context/PhysicalDeviceAttributes.h"
#include "Context/LogicalDevice.h"
#include <algorithm>


namespace uncanny::vulkan
{


FBuffer FRenderDeviceFactory::CreateBuffer() const
{
  return FBuffer{ m_pPhysicalDeviceAttributes, m_pLogicalDevice->GetHandle() };
}


FBottomLevelAS FRenderDeviceFactory::CreateBottomLevelAS() const
{
  return FBottomLevelAS{ this,  m_pLogicalDevice->GetHandle() };
}


FTopLevelAS FRenderDeviceFactory::CreateTopLevelAS() const
{
  return FTopLevelAS{ this,  m_pLogicalDevice->GetHandle() };
}


std::vector<FImage> FRenderDeviceFactory::CreateImages(u32 count) const
{
  std::vector<FImage> images{};
  images.reserve(count);
  for(u32 i = 0; i < count; i++)
  {
    images.push_back({m_pPhysicalDeviceAttributes, m_pLogicalDevice->GetHandle()});
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


FGLSLShaderCompiler FRenderDeviceFactory::CreateGlslShaderCompiler() const
{
  b8 spirv14Supported = UFALSE;
  if (m_pPhysicalDeviceAttributes->IsExtensionPresent(VK_KHR_SPIRV_1_4_EXTENSION_NAME))
  {
    spirv14Supported = UTRUE;
  }
  return FGLSLShaderCompiler{ m_pInstanceAttributes->GetFullVersion(), spirv14Supported };
}


void FRenderDeviceFactory::Initialize(const FInstanceAttributes* pInstanceAttributes,
                                      const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, const FLogicalDevice* pLogicalDevice)
{
  m_pInstanceAttributes = pInstanceAttributes;
  m_pPhysicalDeviceAttributes = pPhysicalDeviceAttributes;
  m_pLogicalDevice = pLogicalDevice;
}


}
