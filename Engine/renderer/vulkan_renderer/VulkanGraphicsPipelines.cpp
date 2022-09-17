
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>
#include <filesystem/FileManager.h>
#include <renderer/Mesh.h>


namespace uncanny
{


b32 FRendererVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  FShaderModulesCreateDependenciesVulkan shaderDeps{};
  shaderDeps.device = mContextPtr->Device();
  shaderDeps.vertexPath = "shaders/colored_mesh.vert.spv";
  shaderDeps.fragmentPath = "shaders/colored_mesh.frag.spv";
  shaderDeps.logInfo = "mesh color sh";

  FGraphicsPipelineCreateDependenciesVulkan createDeps{};
  createDeps.device = mContextPtr->Device();
  createDeps.renderPass = mVkRenderPass;
  createDeps.pShaderDeps = &shaderDeps;
  createDeps.logInfo = "mesh color gp";

  b32 created{ mGraphicsPipeline.create(createDeps) };
  if (not created) {
    UERROR("Could not create graphics pipeline");
    return UFALSE;
  }

  UDEBUG("Created graphics pipelines general!");
  return UTRUE;
}


b32 FRendererVulkan::closeGraphicsPipelinesGeneral() {
  UTRACE("Closing graphics pipelines general...");

  mGraphicsPipeline.close(mContextPtr->Device());

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


}
