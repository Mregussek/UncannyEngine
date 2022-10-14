
#include "RendererVulkan.h"
#include <renderer/vulkan/vulkan_resources/ImageVulkan.h>
#include <utilities/Logger.h>
#include <filesystem/FileManager.h>


namespace uncanny
{


b32 FRendererVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  VkSurfaceFormatKHR renderTargetFormat{ VK_FORMAT_UNDEFINED };
  b32 detectedRenderTarget{
    m_WindowSurface.detectSupportedImageFormat(m_PhysicalDevice.Handle(),
                                               mImageDependencies.renderTarget.formatCandidatesVector,
                                               &renderTargetFormat)
  };
  if (not detectedRenderTarget) {
    UERROR("Could not find suitable render target image format from window surface!");
    return UFALSE;
  }

  VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
  b32 detectedDepth{ detectFormatSupportingFormatFeatures(
      m_PhysicalDevice.Handle(), mImageDependencies.depth.formatCandidatesVector,
      VK_IMAGE_TILING_OPTIMAL, mImageDependencies.depth.formatsFeatureVector, &depthFormat,
      "depth") };
  if (not detectedDepth) {
    UERROR("Could not find suitable depth image format with expected format features!");
    return UFALSE;
  }

  FRenderPassCreateDependenciesVulkan renderPassDeps{};
  renderPassDeps.device = m_LogicalDevice.Handle();
  renderPassDeps.renderTargetFormat = renderTargetFormat.format;
  renderPassDeps.depthFormat = depthFormat;
  renderPassDeps.logInfo = "mesh color rp";

  FShaderModulesCreateDependenciesVulkan shaderDeps{};
  shaderDeps.device = m_LogicalDevice.Handle();
  shaderDeps.vertexPath = "shaders/colored_mesh.vert.spv";
  shaderDeps.fragmentPath = "shaders/colored_mesh.frag.spv";
  shaderDeps.logInfo = "mesh color sh";

  FGraphicsPipelineCreateDependenciesVulkan createDeps{};
  createDeps.device = m_LogicalDevice.Handle();
  createDeps.pRenderPassDeps = &renderPassDeps;
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

  mGraphicsPipeline.close(m_LogicalDevice.Handle());

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


}
