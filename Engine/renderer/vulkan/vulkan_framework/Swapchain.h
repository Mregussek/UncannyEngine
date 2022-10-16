
#ifndef UNCANNYENGINE_SWAPCHAIN_H
#define UNCANNYENGINE_SWAPCHAIN_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>
#include "Image.h"


namespace uncanny::vkf
{


class FWindowSurfaceVulkan;


struct FSwapchainInitDependenciesVulkan {
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  VkDevice logicalDevice{ VK_NULL_HANDLE };
  FWindowSurfaceVulkan* pWindowSurface{ nullptr };
  const std::vector<VkImageUsageFlags>* pPresentableImageUsageVector{ nullptr };
  const std::vector<VkSurfaceFormatKHR>* pFormatCandidatesVector{ nullptr };
  const std::vector<VkFormatFeatureFlags>* pFormatFeaturesVector{ nullptr };
  u32 usedImageCount{ 0 };
};


typedef FSwapchainInitDependenciesVulkan FSwapchainRecreateDependenciesVulkan;


class FSwapchainVulkan {
public:

  b32 init(const FSwapchainInitDependenciesVulkan& deps);
  void terminate(VkDevice device);

  b32 recreate(const FSwapchainRecreateDependenciesVulkan& deps);

  [[nodiscard]] VkSwapchainKHR Handle() const { return m_VkSwapchain; }
  [[nodiscard]] VkSwapchainKHR* HandlePtr() { return &m_VkSwapchain; }
  [[nodiscard]] const std::vector<FImageVulkan>& PresentableImages() const { return m_ImagePresentableVector; }

private:

  VkSwapchainKHR m_VkSwapchain{ VK_NULL_HANDLE };
  VkSwapchainKHR m_VkSwapchainOld{ VK_NULL_HANDLE };
  std::vector<FImageVulkan> m_ImagePresentableVector{};

};


}


#endif //UNCANNYENGINE_SWAPCHAIN_H
