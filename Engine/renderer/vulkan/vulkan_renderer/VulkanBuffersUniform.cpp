
#include "RendererVulkan.h"
#include <renderer/Camera.h>
#include <renderer/vulkan/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan/VulkanUtilities.h>
#include <renderer/vulkan/vulkan_renderer/graphics_pipelines/ShaderModulesVulkan.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createUniformBuffer(FCamera* pCamera, FBufferVulkan* pUBO) {
  UTRACE("Creating uniform buffer...");

  FShaderModuleUniformVulkan uniformObjectShader{};
  uniformObjectShader.matrixMVP = pCamera->retrieveMatrixMVP();

  FBufferCreateDependenciesVulkan createDeps{};
  createDeps.pNext = nullptr;
  createDeps.physicalDevice = mContextPtr->PhysicalDevice();
  createDeps.device = mContextPtr->Device();
  createDeps.size = sizeof(FShaderModuleUniformVulkan);
  createDeps.elemCount = 1;
  createDeps.pData = &uniformObjectShader;
  createDeps.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  createDeps.type = EBufferType::HOST_VISIBLE;
  createDeps.logInfo = "camera uniform";

  b32 createdCameraUBO{ pUBO->create(createDeps) };
  if (not createdCameraUBO) {
    UERROR("Could not create ubo!");
    return UFALSE;
  }

  UDEBUG("Created uniform buffer!");
  return UTRUE;
}


b32 FRendererVulkan::closeUniformBuffer(FBufferVulkan* pUBO) {
  UTRACE("Closing uniform buffer...");

  pUBO->close(mContextPtr->Device());

  UDEBUG("Closed uniform buffer!");
  return UTRUE;
}


}
