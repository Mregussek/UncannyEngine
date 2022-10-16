
#ifndef UNCANNYENGINE_IMAGE_H
#define UNCANNYENGINE_IMAGE_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny::vkf
{


enum class EImageType {
  NONE, PRESENTABLE, DEPTH, RENDER_TARGET
};


struct FImageDataVulkan {
  VkImage handle{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  VkImageView handleView{ VK_NULL_HANDLE };
  VkFramebuffer handleFramebuffer{ VK_NULL_HANDLE };
  VkExtent3D extent{ 0, 0, 0 };
  VkFormat format{ VK_FORMAT_UNDEFINED };
  VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
  VkImageUsageFlags usage{ 0 };
  VkImageAspectFlags aspectMask{ 0 };
  VkImageLayout initialLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
  EImageType type{ EImageType::NONE };
  const char* logInfo{ "" };
};


struct FImageViewCreateDependenciesVulkan {
  b32 shouldCreate{ UFALSE };
};


struct FImageFramebufferCreateDependenciesVulkan {
  b32 shouldCreate{ UFALSE };
  VkRenderPass renderPass{ VK_NULL_HANDLE };
  VkImageView depthImageView{ VK_NULL_HANDLE };
};


struct FImageCreateDependenciesVulkan {
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  VkDevice device{ VK_NULL_HANDLE };
  VkImage handleToUse{ VK_NULL_HANDLE };
  VkExtent3D extent{ 0, 0, 0 };
  VkFormat format{ VK_FORMAT_UNDEFINED };
  VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
  VkImageUsageFlags usage{ 0 };
  VkImageAspectFlags aspectMask{ 0 };
  VkImageLayout initialLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
  EImageType type{ EImageType::NONE };
  FImageViewCreateDependenciesVulkan viewDeps{};
  FImageFramebufferCreateDependenciesVulkan framebufferDeps{};
  const std::vector<VkFormatFeatureFlags>* pFormatsFeaturesToCheck{};
  const char* logInfo{ "" };
};


class FImageVulkan {
public:

  b32 create(const FImageCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  [[nodiscard]] const FImageDataVulkan& getData() const { return mData; }

  [[nodiscard]] b32 isCreated() const { return mCreated and mData.handle != VK_NULL_HANDLE; }

private:

  FImageDataVulkan mData{};
  b32 mCreated{ UFALSE };

};


b32 detectFormatSupportingFormatFeatures(
    VkPhysicalDevice physicalDevice, const std::vector<VkSurfaceFormatKHR>& formatCandidates,
    VkImageTiling tiling, const std::vector<VkFormatFeatureFlags>& formatFeatures,
    VkFormat* pOutFormat, const char* logInfo);


}


#endif //UNCANNYENGINE_IMAGE_H
