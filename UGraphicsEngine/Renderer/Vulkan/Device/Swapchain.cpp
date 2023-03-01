
#include "Swapchain.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan {


struct FSwapchainCreateAttributes {
  std::vector<VkImageUsageFlags> imageUsageFlags{
      VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  };
  std::vector<VkFormatFeatureFlags> imageFormatFeatureFlags{
      VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
      VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
  };
  VkSurfaceFormatKHR surfaceFormat{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  u32 minImageCount{ 2 };
  VkSurfaceTransformFlagBitsKHR preTransform{ VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };
};


static b8 AreCreateAttributesSupported(const FSwapchainCreateAttributes& createAttributes, const FWindowSurface* pWindowSurface);



void FSwapchain::Create(const FLogicalDevice* pLogicalDevice, const FWindowSurface* pWindowSurface) {
  FSwapchainCreateAttributes createAttributes{};
  if (not AreCreateAttributesSupported(createAttributes, pWindowSurface)) {
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Create attributes for swapchain are not supported!");
  }


}


void FSwapchain::Destroy() {

}



b8 AreCreateAttributesSupported(const FSwapchainCreateAttributes& createAttributes, const FWindowSurface* pWindowSurface) {
  const VkSurfaceCapabilitiesKHR& surfaceCapabilities = pWindowSurface->GetCapabilities();

  // Validate surface capabilities...
  // Validating min image count...
  if (not (surfaceCapabilities.minImageCount <= createAttributes.minImageCount and createAttributes.minImageCount <= surfaceCapabilities.maxImageCount)) {
    UERROR("Not supported min image count for surface capabilities!");
    return UFALSE;
  }
  // Validating pre transform...
  if (not (surfaceCapabilities.supportedTransforms & createAttributes.preTransform)) {
    UERROR("Not supported pre transform for surface capabilities!");
    return UFALSE;
  }
  // Validating if all requested image usage flags are supported...
  if (std::ranges::find_if(createAttributes.imageUsageFlags,[supportedFlags = surfaceCapabilities.supportedUsageFlags](auto flag)->bool{
    return supportedFlags & flag; }) == createAttributes.imageUsageFlags.end())
  {
    UERROR("Not supported image usage flag!");
    return UFALSE;
  };
  // ... surface capabilities validated

  // Validate surface format...
  // Validating if requested surface format is supported...
  auto formats = pWindowSurface->GetFormats();
  if (std::ranges::find_if(formats, [expectedFormat = createAttributes.surfaceFormat](auto surfaceFormat)->bool{
    return expectedFormat.format == surfaceFormat.format and expectedFormat.colorSpace == surfaceFormat.colorSpace; }) == formats.end())
  {
    UERROR("Not supported surface format!");
    return UFALSE;
  }
  // Validating if requested surface format features are supported...
  VkFormatProperties formatProperties = pWindowSurface->GetFormatProperties(createAttributes.surfaceFormat.format);
  VkFormatFeatureFlags actualFormatFeatureFlags = createAttributes.imageTiling == VK_IMAGE_TILING_OPTIMAL ? formatProperties.optimalTilingFeatures : formatProperties.linearTilingFeatures;
  if (std::ranges::find_if(createAttributes.imageFormatFeatureFlags, [actualFormatFeatureFlags](VkFormatFeatureFlags flag)->bool{
    return actualFormatFeatureFlags & flag; }) == createAttributes.imageFormatFeatureFlags.end())
  {
    UERROR("Not supported format feature flag!");
    return UFALSE;
  }
  // ... surface format validated

  return UTRUE;
}


}
