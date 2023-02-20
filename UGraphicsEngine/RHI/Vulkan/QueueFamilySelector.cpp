
#include "QueueFamilySelector.h"
#include <algorithm>
#include <map>
#include <functional>
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Utilities.h"


namespace uncanny::vulkan {


typedef std::function<FQueueFamilyScore(VkQueueFamilyProperties, u32, const FInstance&, const FPhysicalDevice&)> GetScoreFunctionObject;


static std::optional<u32> SelectQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                            const GetScoreFunctionObject& getScoreFunctionObject,
                                            const FInstance& instance,
                                            const FPhysicalDevice& physicalDevice);


static FQueueFamilyScore GetGraphicsScore(VkQueueFamilyProperties properties,
                                          u32 queueFamilyIndex,
                                          const FInstance& instance,
                                          const FPhysicalDevice& physicalDevice);


static FQueueFamilyScore GetPresentScore(VkQueueFamilyProperties properties,
                                         u32 queueFamilyIndex,
                                         const FInstance& instance,
                                         const FPhysicalDevice& physicalDevice);


static FQueueFamilyScore GetTransferScore(VkQueueFamilyProperties properties,
                                          u32 queueFamilyIndex,
                                          const FInstance& instance,
                                          const FPhysicalDevice& physicalDevice);


std::optional<u32> FQueueFamilySelector::SelectGraphicsQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                                   const FInstance& instance,
                                                                   const FPhysicalDevice& physicalDevice) const {
  GetScoreFunctionObject getScoreFuncObj = GetGraphicsScore;
  return SelectQueueFamily(queueFamilyProperties, getScoreFuncObj, instance, physicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectPresentQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                                  const FInstance& instance,
                                                                  const FPhysicalDevice& physicalDevice) const {
  GetScoreFunctionObject getScoreFuncObj = GetPresentScore;
  return SelectQueueFamily(queueFamilyProperties, getScoreFuncObj, instance, physicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectTransferQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                                   const FInstance& instance,
                                                                   const FPhysicalDevice& physicalDevice) const {
  GetScoreFunctionObject getScoreFuncObj = GetTransferScore;
  return SelectQueueFamily(queueFamilyProperties, getScoreFuncObj, instance, physicalDevice);
}


std::optional<u32> SelectQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                     const GetScoreFunctionObject& getScoreFunctionObject,
                                     const FInstance& instance,
                                     const FPhysicalDevice& physicalDevice) {
  std::multimap<FQueueFamilyScore, u32> ratings;
  std::ranges::for_each(queueFamilyProperties,
                        [&ratings, getScoreFunctionObject, &instance, &physicalDevice, idx = 0](const VkQueueFamilyProperties& properties) mutable{
                          FQueueFamilyScore score = getScoreFunctionObject(properties, idx, instance, physicalDevice);
                          ratings.insert(std::make_pair(score, idx));
                          ++idx;
                        });
  auto highestRatingIt = ratings.rbegin();
  if (highestRatingIt->first == 0) {
    return std::nullopt;
  }
  return highestRatingIt->second;
}


FQueueFamilyScore GetGraphicsScore(VkQueueFamilyProperties properties,
                                   u32 queueFamilyIndex,
                                   const FInstance& instance,
                                   const FPhysicalDevice& physicalDevice) {
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
    score += 100;
  }
  return score;
}


FQueueFamilyScore GetPresentScore(VkQueueFamilyProperties properties,
                                  u32 queueFamilyIndex,
                                  const FInstance& instance,
                                  const FPhysicalDevice& physicalDevice) {
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
    score += 50;
  }

#ifdef WIN32
  auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
      (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
          instance.GetHandle(), "vkGetPhysicalDeviceWin32PresentationSupportKHR");
  VkBool32 supported = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice.GetHandle(), queueFamilyIndex);
  if (supported) {
    score += 100;
  }
#else
  #error "No support for other platform!"
#endif

  return score;
}


FQueueFamilyScore GetTransferScore(VkQueueFamilyProperties properties,
                                   u32 queueFamilyIndex,
                                   const FInstance& instance,
                                   const FPhysicalDevice& physicalDevice) {
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
    score -= 25;
  }
  if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
    score -= 25;
  }
  if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
    score += 100;
  }
  return score;
}


}
