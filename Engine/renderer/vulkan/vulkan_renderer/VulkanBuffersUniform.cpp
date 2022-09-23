
#include "RendererVulkan.h"
#include <renderer/Camera.h>
#include <renderer/vulkan/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan/VulkanUtilities.h>
#include <renderer/vulkan/vulkan_renderer/graphics_pipelines/ShaderModulesVulkan.h>
#include <utilities/Logger.h>


namespace uncanny
{


static void moveCameraDataToStructUBO(FCamera* pCamera, FShaderModuleUniformVulkan* pOutUBO);


b32 FRendererVulkan::createUniformBuffers(const FRenderSceneConfiguration& sceneConfiguration) {
  UTRACE("Creating uniform buffers...");

  FShaderModuleUniformVulkan uniformObjectShader{};
  moveCameraDataToStructUBO(sceneConfiguration.pCamera, &uniformObjectShader);

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

  b32 createdCameraUBO{ mUniformBufferCamera.create(createDeps) };
  if (not createdCameraUBO) {
    UERROR("Could not create camera ubo!");
    return UFALSE;
  }

  UDEBUG("Created uniform buffers!");
  return UTRUE;
}


b32 FRendererVulkan::closeUniformBuffers() {
  UTRACE("Closing uniform buffers...");

  mUniformBufferCamera.close(mContextPtr->Device());

  UDEBUG("Closed uniform buffers!");
  return UTRUE;
}


void moveCameraDataToStructUBO(FCamera* pCamera, FShaderModuleUniformVulkan* pOutUBO) {
  UTRACE("Moving camera data to UBO struct...");
  pOutUBO->matrixMVP = pCamera->retrieveMatrixMVP();
}


}
