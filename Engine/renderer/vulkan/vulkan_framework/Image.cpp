
#include "Image.h"
#include "Memory.h"
#include "Utilities.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


static b32 areFormatsFeaturesDependenciesMetForImageFormat(
    VkFormat imageFormat, VkImageTiling tiling, VkPhysicalDevice physicalDevice,
    const std::vector<VkFormatFeatureFlags>& formatFeatureVector, const char* logInfo);


static b32 areTilingFeaturesMetForImageFormat(
    VkFormatFeatureFlags deviceFormatFeatureFlags,
    const std::vector<VkFormatFeatureFlags>& formatFeatureDependenciesVector);


b32 FImageVulkan::create(const FImageCreateDependenciesVulkan& deps) {
  UTRACE("Creating image...");
  if (mCreated) {
    UERROR("Image is already created, could not continue...");
    return UFALSE;
  }
  mCreated = UTRUE;

  if (deps.type == EImageType::PRESENTABLE) {
    mData.handle = deps.handleToUse;
  }
  else {
    mData.handle = VK_NULL_HANDLE;
  }
  mData.deviceMemory = VK_NULL_HANDLE;
  mData.handleView = VK_NULL_HANDLE;
  mData.handleFramebuffer = VK_NULL_HANDLE;
  mData.extent = { deps.extent.width, deps.extent.height, 1 };
  mData.format = deps.format;
  mData.tiling = deps.tiling;
  mData.usage = deps.usage;
  mData.aspectMask = deps.aspectMask;
  mData.initialLayout = deps.initialLayout;
  mData.type = deps.type;
  mData.logInfo = deps.logInfo;

  b32 featuresAreSupported{ areFormatsFeaturesDependenciesMetForImageFormat(
      deps.format, deps.tiling, deps.physicalDevice,
      *deps.pFormatsFeaturesToCheck, "depth") };
  if (not featuresAreSupported) {
    UERROR("Could not create depth images, as format features are not supported!");
    return UFALSE;
  }

  if (mData.type != EImageType::PRESENTABLE) {
    UTRACE("Creating {} image handle...", mData.logInfo);
    VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = mData.format;
    imageCreateInfo.extent = mData.extent;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = mData.tiling;
    imageCreateInfo.usage = mData.usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.initialLayout = mData.initialLayout;

    AssertResultVulkan( vkCreateImage(deps.device, &imageCreateInfo, nullptr, &mData.handle) );

    UTRACE("Allocating {} image device memory...", mData.logInfo);
    VkMemoryRequirements memoryReqs{};
    vkGetImageMemoryRequirements(deps.device, mData.handle, &memoryReqs);

    FMemoryAllocationDependenciesVulkan allocDeps{};
    allocDeps.physicalDevice = deps.physicalDevice;
    allocDeps.device = deps.device;
    allocDeps.memoryRequirements = memoryReqs;
    allocDeps.propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocDeps.logInfo = mData.logInfo;

    b32 allocated{ FMemoryVulkan::allocate(allocDeps, &mData.deviceMemory) };
    if (not allocated) {
      UERROR("Could not allocate {} image memory!", mData.logInfo);
      return UFALSE;
    }
    FMemoryVulkan::bindImage(deps.device, mData.handle, mData.deviceMemory);
  }
  else {
    UWARN("Skipping {} image creation via vkCreateImage as image is owned by swapchain and its "
          "allocation!", mData.logInfo);
  }

  if (not deps.viewDeps.shouldCreate) { // if you should not create imageview
    UWARN("As image view is not needed for {}, skipping imageview and framebuffer creation!",
          mData.logInfo);
    return UTRUE;
  }

  UTRACE("Creating image view for {} image...", mData.logInfo);
  VkComponentMapping componentMapping{};
  componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = mData.aspectMask;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.image = mData.handle;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = mData.format;
  imageViewCreateInfo.components = componentMapping;
  imageViewCreateInfo.subresourceRange = imageSubresourceRange;

  AssertResultVulkan( vkCreateImageView(deps.device, &imageViewCreateInfo, nullptr,
                                             &mData.handleView) );

  if (not deps.framebufferDeps.shouldCreate) { // if you should not create framebuffer
    UWARN("As framebuffer is not needed for {}, skipping framebuffer creation!", mData.logInfo);
    return UTRUE;
  }

  UTRACE("Creating framebuffer for {} image...", mData.logInfo);
  std::array<VkImageView, 2> attachments{ mData.handleView, deps.framebufferDeps.depthImageView };

  VkFramebufferCreateInfo framebufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
  framebufferCreateInfo.pNext = nullptr;
  framebufferCreateInfo.flags = 0;
  framebufferCreateInfo.renderPass = deps.framebufferDeps.renderPass;
  framebufferCreateInfo.attachmentCount = attachments.size();
  framebufferCreateInfo.pAttachments = attachments.data();
  framebufferCreateInfo.width = mData.extent.width;
  framebufferCreateInfo.height = mData.extent.height;
  framebufferCreateInfo.layers = 1;

  AssertResultVulkan( vkCreateFramebuffer(deps.device, &framebufferCreateInfo, nullptr,
                                               &mData.handleFramebuffer) );
  UDEBUG("Created image along with image view and framebuffer for {}!", mData.logInfo);
  return UTRUE;
}


b32 FImageVulkan::close(VkDevice device) {
  if (mData.handleFramebuffer != VK_NULL_HANDLE) {
    UTRACE("Destroying {} framebuffer...", mData.logInfo);
    vkDestroyFramebuffer(device, mData.handleFramebuffer, nullptr);
  }
  else {
    UWARN("As {} framebuffer is not created, it is not destroyed!", mData.logInfo);
  }

  if (mData.handleView != VK_NULL_HANDLE) {
    UTRACE("Destroying {} image view...", mData.logInfo);
    vkDestroyImageView(device, mData.handleView, nullptr);
  }
  else {
    UWARN("As {} image view is not created, it is not destroyed!", mData.logInfo);
  }

  // It is nice to firstly destroy image, then free its memory, as
  // if image will be used it will be referencing freed memory
  if (mData.handle != VK_NULL_HANDLE and mData.type != EImageType::PRESENTABLE) {
    UTRACE("Destroying {} image...", mData.logInfo);
    vkDestroyImage(device, mData.handle, nullptr);
  }
  else {
    if (mData.type != EImageType::PRESENTABLE) {
      UWARN("As {} image is not created, it is not destroyed!", mData.logInfo);
    }
    else {
      UTRACE("As {} image handle is handled by swapchain, it is not destroyed here!",
             mData.logInfo);
    }
  }

  if (mData.deviceMemory != VK_NULL_HANDLE) {
    UTRACE("Freeing {} image memory...", mData.logInfo);
    vkFreeMemory(device, mData.deviceMemory, nullptr);
  }
  else {
    if (mData.type != EImageType::PRESENTABLE) {
      UWARN("As {} image memory is not allocated, it won't be freed!", mData.logInfo);
    }
    else {
      UTRACE("As {} image memory is handled by swapchain, it is not freed here!", mData.logInfo);
    }
  }

  UTRACE("Clearing other data members of image {}...", mData.logInfo);
  mData = {};
  mCreated = UFALSE;

  return UTRUE;
}


b32 detectFormatSupportingFormatFeatures(
    VkPhysicalDevice physicalDevice, const std::vector<VkSurfaceFormatKHR>& formatCandidates,
    VkImageTiling tiling, const std::vector<VkFormatFeatureFlags>& formatFeatures,
    VkFormat* pOutFormat, const char* logInfo) {
  for (VkSurfaceFormatKHR candidate : formatCandidates) {
    b32 supported{ areFormatsFeaturesDependenciesMetForImageFormat(candidate.format, tiling,
                                                                   physicalDevice,
                                                                   formatFeatures, logInfo) };
    if (supported) {
      UDEBUG("Found {} {} format supporting format features!", logInfo, candidate.format);
      *pOutFormat = candidate.format;
      return UTRUE;
    }
  }

  UERROR("Could not find proper {} candidate supporting format features", logInfo);
  return UFALSE;
}


b32 areFormatsFeaturesDependenciesMetForImageFormat(
    VkFormat imageFormat, VkImageTiling tiling, VkPhysicalDevice physicalDevice,
    const std::vector<VkFormatFeatureFlags>& formatFeatureVector, const char* logInfo) {
  UTRACE("Validating {} image format features dependencies...", logInfo);

  VkFormatProperties formatProperties{};
  vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

  if (tiling == VK_IMAGE_TILING_OPTIMAL) {
    b32 tilingFeaturesSupported{ areTilingFeaturesMetForImageFormat(
        formatProperties.optimalTilingFeatures, formatFeatureVector) };
    if (not tilingFeaturesSupported) {
      UERROR("Some Optimal Format Feature Flag is not supported by format {} for {} image!",
             imageFormat, logInfo);
      return UFALSE;
    }
  }
  else {
    b32 tilingFeaturesSupported{ areTilingFeaturesMetForImageFormat(
        formatProperties.linearTilingFeatures, formatFeatureVector) };
    if (not tilingFeaturesSupported) {
      UERROR("Some Linear Format Feature Flag is not supported by format {} for {} image!",
             imageFormat, logInfo);
      return UFALSE;
    }
  }

  UDEBUG("Validated {} image format features dependencies, it seems correct!", logInfo);
  return UTRUE;
}


b32 areTilingFeaturesMetForImageFormat(
    VkFormatFeatureFlags deviceFormatFeatureFlags,
    const std::vector<VkFormatFeatureFlags>& formatFeatureDependenciesVector) {
  for (VkFormatFeatureFlags featureFlag : formatFeatureDependenciesVector) {
    if (not (deviceFormatFeatureFlags & featureFlag)) {
      UERROR("Format Feature flag {} is not supported!", featureFlag);
      return UFALSE;
    }
  }

  UTRACE("Found every format feature flag supported!");
  return UTRUE;
}


}
