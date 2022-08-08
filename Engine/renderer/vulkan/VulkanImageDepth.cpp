
#include "RenderContextVulkan.h"
#include "VulkanImageDepth.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createDepthImages() {
  UTRACE("Creating depth images...");

  if (mVkDepthFormat == VK_FORMAT_UNDEFINED) {
    UTRACE("Depth format is not detected! Searching...");
    b32 detectedDepthFormat{
      detectSupportedDepthFormat(mVkPhysicalDevice,
                                 mPhysicalDeviceDependencies.depthFormatDependencies,
                                 &mVkDepthFormat) };
    if (not detectedDepthFormat) {
      UERROR("Could not detect depth format!");
      return UFALSE;
    }

    UTRACE("Found depth format, now can create depth images...");
  }



  UDEBUG("Created depth images!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeDepthImages() {
  UTRACE("Closing depth images...");

  UDEBUG("Closed depth images!");
  return UTRUE;
}


b32 detectSupportedDepthFormat(VkPhysicalDevice physicalDevice,
                               const std::vector<VkFormat>& depthFormatCandidates,
                               VkFormat* pOutDepthFormat) {
  VkFormatFeatureFlagBits flags{ VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT };
  for (u32 i = 0; i < depthFormatCandidates.size(); i++) {
    VkFormatProperties formatProperties{};
    vkGetPhysicalDeviceFormatProperties(physicalDevice, depthFormatCandidates[i],
                                        &formatProperties);

    if ((formatProperties.linearTilingFeatures & flags) == flags) {
      UTRACE("Detected depth format at index {}!", i);
      *pOutDepthFormat = depthFormatCandidates[i];
      return UTRUE;
    }
    else if ((formatProperties.optimalTilingFeatures & flags) == flags) {
      UTRACE("Detected depth format at index {}!", i);
      *pOutDepthFormat = depthFormatCandidates[i];
      return UTRUE;
    }
  }

  UERROR("Could not detect depth format from candidates!");
  return UFALSE;
}


}
