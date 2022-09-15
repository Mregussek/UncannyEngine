
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <renderer/vulkan_context/VulkanWindowSurface.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createDepthImage() {
  UTRACE("Creating depth image...");

  // TODO: Make sure no other format be used
  UTRACE("Using 0-indexed depth format from dependencies!");
  VkFormat imageFormat{ mImageDependencies.depth.formatCandidatesVector[0].format };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };

  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : mImageDependencies.depth.usageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  VkExtent3D surfaceExtent3D{ mContextPtr->SurfaceExtent().width,
                              mContextPtr->SurfaceExtent().height,
                              1 };

  FImageCreateDependenciesVulkan createDeps{};
  createDeps.physicalDevice = mContextPtr->PhysicalDevice();
  createDeps.device = mContextPtr->Device();
  createDeps.extent = surfaceExtent3D;
  createDeps.format = imageFormat;
  createDeps.tiling = imageTiling;
  createDeps.usage = imageUsage;
  createDeps.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  createDeps.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  createDeps.type = EImageType::DEPTH;
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

  b32 closedProperly{ mDepthImage.close(mContextPtr->Device()) };
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
