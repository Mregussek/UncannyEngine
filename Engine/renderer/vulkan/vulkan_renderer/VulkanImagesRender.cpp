
#include "RendererVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createRenderTargetImages() {
  u32 imageCount{ static_cast<u32>(mImagePresentableVector.size()) };
  UTRACE("Creating {} Render Target Images...", imageCount);

  if (not mDepthImage.isCreated()) {
    UERROR("Could not create render target image as depth image is not yet created!");
    return UFALSE;
  }

  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };

  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : mImageDependencies.renderTarget.usageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  mImageRenderTargetVector.resize(imageCount);

  VkExtent3D surfaceExtent3D{ m_WindowSurface.Extent().width,
                              m_WindowSurface.Extent().height,
                              1 };

  FImageCreateDependenciesVulkan createDeps{};
  createDeps.physicalDevice = m_PhysicalDevice.Handle();
  createDeps.device = m_LogicalDevice.Handle();
  createDeps.extent = surfaceExtent3D;
  createDeps.format = mGraphicsPipeline.getRenderPassData().renderTargetFormat;
  createDeps.tiling = imageTiling;
  createDeps.usage = imageUsage;
  createDeps.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createDeps.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
  createDeps.type = EImageType::RENDER_TARGET;
  createDeps.viewDeps.shouldCreate = UTRUE;
  createDeps.framebufferDeps.shouldCreate = UTRUE;
  createDeps.framebufferDeps.renderPass = mGraphicsPipeline.getRenderPassData().renderPass;
  createDeps.framebufferDeps.depthImageView = mDepthImage.getData().handleView;
  createDeps.pFormatsFeaturesToCheck = &mImageDependencies.renderTarget.formatsFeatureVector;
  createDeps.logInfo = "render target";

  for (u32 i = 0; i < imageCount; i++) {
    UTRACE("Creating render target image {}...", i);
    b32 created{ mImageRenderTargetVector[i].create(createDeps) };
    if (not created) {
      UERROR("Could not create render target image at index {}", i);
      return UFALSE;
    }
    UDEBUG("Successfully created render target image!");
  }

  UDEBUG("Created Render Target Images!");
  return UTRUE;
}


b32 FRendererVulkan::closeRenderTargetImages() {
  UTRACE("Closing Render Target Images...");

  if (mImageRenderTargetVector.empty()) {
    UWARN("As render target image vector is not initialized (it is empty), not closing anything!");
    return UTRUE;
  }

  for (FImageVulkan& renderTargetImage : mImageRenderTargetVector) {
    b32 closedProperly{ renderTargetImage.close(m_LogicalDevice.Handle()) };
    if (not closedProperly) {
      UERROR("Could not close properly render target image!");
      return UFALSE;
    }
  }
  mImageRenderTargetVector.clear();

  UDEBUG("Closed Render Target Images!");
  return UTRUE;
}


b32 FRendererVulkan::recreateRenderTargetImages() {
  UTRACE("Recreating render target images...");

  b32 closed{ closeRenderTargetImages() };
  if (not closed) {
    UERROR("Could not close render target images, some issue appeared!");
    return UFALSE;
  }

  b32 created{ createRenderTargetImages() };
  if (not created) {
    UERROR("Could not create render target images during recreation!");
    return UFALSE;
  }

  UDEBUG("Recreated render target images!");
  return UTRUE;
}


}
