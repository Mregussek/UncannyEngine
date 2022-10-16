
#include "RendererVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createDepthImage() {
  UTRACE("Creating depth image...");

  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : mImageDependencies.depth.usageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  VkExtent3D surfaceExtent3D{ m_WindowSurface.Extent().width,
                              m_WindowSurface.Extent().height,
                              1 };

  vkf::FImageCreateDependenciesVulkan createDeps{};
  createDeps.physicalDevice = m_PhysicalDevice.Handle();
  createDeps.device = m_LogicalDevice.Handle();
  createDeps.extent = surfaceExtent3D;
  createDeps.format = mGraphicsPipeline.getRenderPassData().depthFormat;
  createDeps.tiling = VK_IMAGE_TILING_OPTIMAL;
  createDeps.usage = imageUsage;
  createDeps.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  createDeps.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  createDeps.type = vkf::EImageType::DEPTH;
  createDeps.viewDeps.shouldCreate = UTRUE;
  createDeps.framebufferDeps.shouldCreate = UFALSE;
  createDeps.pFormatsFeaturesToCheck = &mImageDependencies.depth.formatsFeatureVector;
  createDeps.logInfo = "depth";

  b32 created{ mDepthImage.create(createDeps) };
  if (not created) {
    UERROR("Could not create depth image!");
    return UFALSE;
  }

  // TODO: add transition to VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL

  UDEBUG("Created depth image!");
  return UTRUE;
}


b32 FRendererVulkan::closeDepthImage() {
  UTRACE("Closing depth image...");

  b32 closedProperly{ mDepthImage.close(m_LogicalDevice.Handle()) };
  if (not closedProperly) {
    UERROR("Could not close properly depth image!");
    return UFALSE;
  }

  UDEBUG("Closed depth image!");
  return UTRUE;
}


b32 FRendererVulkan::recreateDepthImage() {
  UTRACE("Recreating depth image...");

  b32 closed{ closeDepthImage() };
  if (not closed) {
    UERROR("Could not close depth image, some issue appeared!");
    return UFALSE;
  }

  b32 created{ createDepthImage() };
  if (not created) {
    UERROR("Could not create depth image during recreation!");
    return UFALSE;
  }

  UDEBUG("Recreated depth image!");
  return UTRUE;
}


}
