
#include "RendererVulkan.h"
#include <renderer/vulkan/vulkan_renderer/graphics_pipelines/ShaderModulesVulkan.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createUniformBuffer(FShaderModuleUniformVulkan* pShaderUniform,
                                         vkf::FBufferVulkan* pBuffer) {
  UTRACE("Creating uniform buffer...");

  vkf::FBufferCreateDependenciesVulkan createDeps{};
  createDeps.pNext = nullptr;
  createDeps.physicalDevice = m_PhysicalDevice.Handle();
  createDeps.device = m_LogicalDevice.Handle();
  createDeps.size = sizeof(FShaderModuleUniformVulkan);
  createDeps.elemCount = 1;
  createDeps.pData = pShaderUniform;
  createDeps.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  createDeps.type = vkf::EBufferType::HOST_VISIBLE;
  createDeps.logInfo = "camera uniform";

  b32 createdCameraUBO{ pBuffer->create(createDeps) };
  if (not createdCameraUBO) {
    UERROR("Could not create ubo!");
    return UFALSE;
  }

  UDEBUG("Created uniform buffer!");
  return UTRUE;
}


b32 FRendererVulkan::closeUniformBuffer(vkf::FBufferVulkan* pBuffer) {
  UTRACE("Closing uniform buffer...");

  pBuffer->close(m_LogicalDevice.Handle());

  UDEBUG("Closed uniform buffer!");
  return UTRUE;
}


}
