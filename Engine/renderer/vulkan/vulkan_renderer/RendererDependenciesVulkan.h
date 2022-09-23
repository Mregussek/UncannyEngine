
#ifndef UNCANNYENGINE_RENDERERDEPENDENCIESVULKAN_H
#define UNCANNYENGINE_RENDERERDEPENDENCIESVULKAN_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


struct FSpecificImageDependencies {
  std::vector<VkSurfaceFormatKHR> formatCandidatesVector{};
  std::vector<VkFormatFeatureFlags> formatsFeatureVector{};
  std::vector<VkImageUsageFlags> usageVector{};
};


struct FImagesDependencies {
  FSpecificImageDependencies renderTarget{};
  FSpecificImageDependencies depth{};
};


struct FSwapchainDependencies {
  // @brief count of images used
  u32 usedImageCount{ UUNUSED };
  // @brief image format dependencies
  std::vector<VkSurfaceFormatKHR> formatCandidatesVector{};
  // @brief image usage dependencies
  std::vector<VkImageUsageFlags> imageUsageVector{};
  // @brief image format features dependencies
  std::vector<VkFormatFeatureFlags> imageFormatFeatureVector{};
};


}


#endif //UNCANNYENGINE_RENDERERDEPENDENCIESVULKAN_H
