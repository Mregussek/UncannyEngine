
#include "Swapchain.h"
#include "WindowSurface.h"
#include "Utilities.h"
#include <utilities/Logger.h>


namespace uncanny::vkf
{


static b32 areDependenciesCorrect(const FSwapchainInitDependenciesVulkan& deps);


static void destroySwapchain(VkSwapchainKHR* pSwapchain, VkDevice device, const char* pSwapchainLogType);


b32 FSwapchainVulkan::init(const FSwapchainInitDependenciesVulkan& deps) {
  UTRACE("Initializing swapchain vulkan...");

  // Validate dependencies...
  b32 properSwapchainDependencies{ areDependenciesCorrect(deps) };
  if (not properSwapchainDependencies) {
    UERROR("Wrong swapchain dependencies passed for creation, cannot acquire images nor present!");
    return UFALSE;
  }

  VkSurfaceFormatKHR imageFormat{ VK_FORMAT_UNDEFINED };
  b32 detected{ deps.pWindowSurface->detectSupportedImageFormat(
      deps.physicalDevice, *deps.pFormatCandidatesVector, &imageFormat) };
  if (not detected) {
    UERROR("Could not find suitable swapchain presentable image format from window surface!");
    return UFALSE;
  }

  // OR every image usage from dependencies
  VkImageUsageFlags imageUsage{ 0 };
  for (VkImageUsageFlags imageUsageFlag : *deps.pPresentableImageUsageVector) {
    imageUsage = imageUsage | imageUsageFlag;
  }

  VkExtent2D imageExtent2D{ deps.pWindowSurface->Extent() };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };

  VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = deps.pWindowSurface->Handle();
  createInfo.minImageCount = deps.usedImageCount;
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
  createInfo.presentMode = deps.pWindowSurface->PresentMode();
  createInfo.clipped = VK_TRUE; // clipping world that is beyond presented surface (not visible)
  createInfo.oldSwapchain = m_VkSwapchainOld;

  AssertResultVulkan( vkCreateSwapchainKHR(deps.logicalDevice, &createInfo, nullptr, &m_VkSwapchain) );

  // Retrieving presentable images from swapchain...
  u32 imageCount{ 0 };
  vkf::AssertResultVulkan( vkGetSwapchainImagesKHR(deps.logicalDevice, m_VkSwapchain, &imageCount, nullptr) );
  std::vector<VkImage> imageVector(imageCount);
  vkf::AssertResultVulkan( vkGetSwapchainImagesKHR(deps.logicalDevice, m_VkSwapchain, &imageCount, imageVector.data()) );

  // Copying retrieved swapchain images into presentable member handles...
  m_ImagePresentableVector.resize(imageCount);

  FImageCreateDependenciesVulkan createDeps{};
  createDeps.physicalDevice = deps.physicalDevice;
  createDeps.device = deps.logicalDevice;
  // createDeps.handleToUse will be filled later
  createDeps.extent = { imageExtent2D.width, imageExtent2D.height, 1 };
  createDeps.format = imageFormat.format;
  createDeps.tiling = imageTiling;
  createDeps.usage = imageUsage;
  createDeps.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createDeps.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  createDeps.type = vkf::EImageType::PRESENTABLE;
  createDeps.viewDeps.shouldCreate = UFALSE;
  createDeps.framebufferDeps.shouldCreate = UFALSE;
  createDeps.pFormatsFeaturesToCheck = deps.pFormatFeaturesVector;
  createDeps.logInfo = "presentable swapchain";

  for(u32 i = 0; i < imageCount; i++) {
    UTRACE("Working on swapchain presentable image {}...", i);
    createDeps.handleToUse = imageVector[i];
    m_ImagePresentableVector[i].create(createDeps);
  }
  imageVector.clear();

  UDEBUG("Initialized swapchain vulkan!");
  return UTRUE;
}


void FSwapchainVulkan::terminate(VkDevice device) {
  UTRACE("Terminating swapchain vulkan...");

  for (FImageVulkan& image : m_ImagePresentableVector) {
    image.close(device);
  }
  m_ImagePresentableVector.clear();

  destroySwapchain(&m_VkSwapchain, device, "Current");
  destroySwapchain(&m_VkSwapchainOld, device, "Old");

  UDEBUG("Terminated swapchain vulkan!");
}


b32 FSwapchainVulkan::recreate(const FSwapchainRecreateDependenciesVulkan& deps) {
  UTRACE("Recreating swapchain vulkan...");

  // Firstly destroying images and its image views as they are not needed and
  // those variables will be filled during createSwapchain()
  for (FImageVulkan& image : m_ImagePresentableVector) {
    image.close(deps.logicalDevice);
  }
  m_ImagePresentableVector.clear();

  // query new information about window surface capabilities
  deps.pWindowSurface->updateCapabilities(deps.physicalDevice);

  // reassigning variables, where old swapchain will be used to creation new one
  m_VkSwapchainOld = m_VkSwapchain;
  m_VkSwapchain = VK_NULL_HANDLE;

  // creating new swapchain and its images with image views
  b32 properlyCreatedSwapchain{ init(deps) };
  if (not properlyCreatedSwapchain) {
    UERROR("Could not recreate swapchain!");
    return UFALSE;
  }

  // destroying old swapchain
  destroySwapchain(&m_VkSwapchainOld, deps.logicalDevice, "OldRecreate");

  UDEBUG("Recreated swapchain vulkan!");
  return UTRUE;
}


b32 areDependenciesCorrect(const FSwapchainInitDependenciesVulkan& deps) {
  UTRACE("Validating swapchain dependencies, if they are proper...");

  // make sure that surface caps support used image count...
  VkSurfaceCapabilitiesKHR surfaceCaps{ deps.pWindowSurface->Capabilities() };
  u32 minCount{ surfaceCaps.minImageCount };
  u32 maxCount{ surfaceCaps.maxImageCount };
  if (not (minCount <= deps.usedImageCount and deps.usedImageCount <= maxCount)) {
    UERROR("There is no required image count for swapchain! Cannot create swapchain! Min: {}, "
           "Max: {}, Expected: {}", minCount, maxCount, deps.usedImageCount);
    return UFALSE;
  }

  // validate image usage
  for (VkImageUsageFlags dependencyImageUsage : *deps.pPresentableImageUsageVector) {
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

  UTRACE("Validated swapchain dependencies, they are proper!");
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
