
#include "RendererVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <renderer/vulkan/vulkan_context/ContextVulkan.h>
#include <utilities/Logger.h>
#include <window/Window.h>


namespace uncanny
{


static void destroySwapchain(VkSwapchainKHR* pSwapchain, VkDevice device,
                             const char*pSwapchainLogType);


b32 FRendererVulkan::areSwapchainDependenciesCorrect() {
  UTRACE("Validating swapchain dependencies...");

  auto surfaceCaps{ mContextPtr->SurfaceCapabilities() };
  // make sure that surface caps support used image count...
  u32 minCount{ surfaceCaps.minImageCount };
  u32 maxCount{ surfaceCaps.maxImageCount };
  u32 usedCount{ mSwapchainDependencies.usedImageCount };
  if (not (minCount <= usedCount and usedCount <= maxCount)) {
    UERROR("There is no required image count for swapchain! Cannot create swapchain! Min: {}, "
           "Max: {}, Expected: {}", minCount, maxCount, usedCount);
    return UFALSE;
  }

  // validate image usage
  for (VkImageUsageFlags dependencyImageUsage : mSwapchainDependencies.imageUsageVector) {
    if (not (surfaceCaps.supportedUsageFlags & dependencyImageUsage)) {
      UERROR("{} image usage is not supported!", dependencyImageUsage);
      return UFALSE;
    }
  }

  // validate pre-transform
  if (not (surfaceCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)) {
    UERROR("Identity surface pre transform is not supported!");
    return UFALSE;
  }

  UDEBUG("Validated swapchain dependencies, everything is fine!");
  return UTRUE;
}


b32 FRendererVulkan::createSwapchain() {
  UTRACE("Creating swapchain...");

  b32 properSwapchainDependencies{ areSwapchainDependenciesCorrect() };
  if (not properSwapchainDependencies) {
    UERROR("Wrong swapchain dependencies passed for creation, cannot acquire images nor present!");
    return UFALSE;
  }

  VkSurfaceFormatKHR imageFormat{ VK_FORMAT_UNDEFINED };
  VkExtent2D imageExtent2D{ mContextPtr->SurfaceExtent() };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };

  b32 detected{ mContextPtr->detectSupportedImageFormatByWindowSurface(
      mSwapchainDependencies.formatCandidatesVector, &imageFormat) };
  if (not detected) {
    UERROR("Could not find suitable swapchain presentable image format from window surface!");
    return UFALSE;
  }

  // OR every image usage from dependencies
  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : mSwapchainDependencies.imageUsageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = mContextPtr->Surface();
  createInfo.minImageCount = mSwapchainDependencies.usedImageCount;
  createInfo.imageFormat = imageFormat.format;
  createInfo.imageColorSpace = imageFormat.colorSpace;
  createInfo.imageExtent = imageExtent2D;
  createInfo.imageArrayLayers = 1; // non-stereoscopic-3D app
  createInfo.imageUsage = imageUsage;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // images are exclusive to queue family
  createInfo.queueFamilyIndexCount = 0;      // for exclusive sharing mode, param is ignored
  createInfo.pQueueFamilyIndices = nullptr; // for exclusive sharing mode, param is ignored
  createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // no transparency with OS
  createInfo.presentMode = mContextPtr->SurfacePresentMode();
  createInfo.clipped = VK_TRUE; // clipping world that is beyond presented surface (not visible)
  createInfo.oldSwapchain = mVkSwapchainOld;

  VkResult createdSwapchain{ vkCreateSwapchainKHR(mContextPtr->Device(), &createInfo, nullptr,
                                                  &mVkSwapchainCurrent) };
  if (createdSwapchain != VK_SUCCESS) {
    UERROR("Could not create swapchain!");
    return UFALSE;
  }

  UTRACE("Retrieving presentable images from swapchain...");
  u32 imageCount{ 0 };
  U_VK_ASSERT( vkGetSwapchainImagesKHR(mContextPtr->Device(), mVkSwapchainCurrent, &imageCount,
                                       nullptr) );
  std::vector<VkImage> imageVector(imageCount);
  U_VK_ASSERT( vkGetSwapchainImagesKHR(mContextPtr->Device(), mVkSwapchainCurrent, &imageCount,
                                       imageVector.data()) );

  // Copying retrieved swapchain images into presentable member handles...
  mImagePresentableVector.resize(imageCount);

  FImageCreateDependenciesVulkan createDeps{};
  createDeps.physicalDevice = mContextPtr->PhysicalDevice();
  createDeps.device = mContextPtr->Device();
  // createDeps.handleToUse will be filled later
  createDeps.extent = { imageExtent2D.width, imageExtent2D.height, 1 };
  createDeps.format = imageFormat.format;
  createDeps.tiling = imageTiling;
  createDeps.usage = imageUsage;
  createDeps.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createDeps.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  createDeps.type = EImageType::PRESENTABLE;
  createDeps.viewDeps.shouldCreate = UFALSE;
  createDeps.framebufferDeps.shouldCreate = UFALSE;
  createDeps.pFormatsFeaturesToCheck = &mSwapchainDependencies.imageFormatFeatureVector;
  createDeps.logInfo = "presentable swapchain";

  for(u32 i = 0; i < imageCount; i++) {
    UTRACE("Working on swapchain presentable image {}...", i);
    createDeps.handleToUse = imageVector[i];
    mImagePresentableVector[i].create(createDeps);
  }
  imageVector.clear();

  UDEBUG("Created swapchain!");
  return UTRUE;
}


b32 FRendererVulkan::closeSwapchain() {
  UTRACE("Closing swapchain...");

  for (FImageVulkan& image : mImagePresentableVector) {
    image.close(mContextPtr->Device());
  }
  mImagePresentableVector.clear();

  destroySwapchain(&mVkSwapchainCurrent, mContextPtr->Device(), "Current");
  destroySwapchain(&mVkSwapchainOld, mContextPtr->Device(), "Old");

  UDEBUG("Closed swapchain!");
  return UTRUE;
}


b32 FRendererVulkan::recreateSwapchain() {
  UTRACE("Recreating swapchain...");

  // Firstly destroying images and its image views as they are not needed and
  // those variables will be filled during createSwapchain()
  for (FImageVulkan& image : mImagePresentableVector) {
    image.close(mContextPtr->Device());
  }
  mImagePresentableVector.clear();

  // query new information about window surface capabilities
  b32 collectedCapabilities{ mContextPtr->collectWindowSurfaceCapabilities() };
  if (not collectedCapabilities) {
    UERROR("Could not update window surface capabilities!");
    return UFALSE;
  }

  // reassigning variables, where old swapchain will be used to creation new one
  mVkSwapchainOld = mVkSwapchainCurrent;
  mVkSwapchainCurrent = VK_NULL_HANDLE;

  // creating new swapchain and its images with image views
  b32 properlyCreatedSwapchain{ createSwapchain() };
  if (not properlyCreatedSwapchain) {
    UERROR("Could not recreate swapchain!");
    return UFALSE;
  }

  // destroying old swapchain
  destroySwapchain(&mVkSwapchainOld, mContextPtr->Device(), "OldRecreate");

  UDEBUG("Recreated swapchain!");
  return UTRUE;
}


void destroySwapchain(VkSwapchainKHR* pSwapchain, VkDevice device, const char* pSwapchainLogType) {
  if (*pSwapchain != VK_NULL_HANDLE) {
    UTRACE("Destroying {} swapchain...", pSwapchainLogType);
    vkDestroySwapchainKHR(device, *pSwapchain, nullptr);
    *pSwapchain = VK_NULL_HANDLE;
  }
  else {
    UWARN("{} swapchain is not created, so it won't be destroyed!", pSwapchainLogType);
  }
}


}
