
#include "QueueFamilySelector.h"
#include <algorithm>
#include <map>
#include <functional>
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Utilities.h"


namespace uncanny::vulkan {


typedef std::function<FQueueFamilyScore(VkQueueFamilyProperties, u32, VkInstance, VkPhysicalDevice)> GetScoreFunctionObject;


static std::optional<u32> SelectQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                            const GetScoreFunctionObject& getScoreFunctionObject,
                                            VkInstance vkInstance,
                                            VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetGraphicsScore(VkQueueFamilyProperties properties,
                                          u32 queueFamilyIndex,
                                          VkInstance vkInstance,
                                          VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetPresentScore(VkQueueFamilyProperties properties,
                                         u32 queueFamilyIndex,
                                         VkInstance vkInstance,
                                         VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetTransferScore(VkQueueFamilyProperties properties,
                                          u32 queueFamilyIndex,
                                          VkInstance vkInstance,
                                          VkPhysicalDevice vkPhysicalDevice);


std::optional<u32> FQueueFamilySelector::SelectGraphicsQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                                   VkInstance vkInstance,
                                                                   VkPhysicalDevice vkPhysicalDevice) const {
  GetScoreFunctionObject getScoreFuncObj = GetGraphicsScore;
  return SelectQueueFamily(queueFamilyProperties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectPresentQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                                  VkInstance vkInstance,
                                                                  VkPhysicalDevice vkPhysicalDevice) const {
  GetScoreFunctionObject getScoreFuncObj = GetPresentScore;
  return SelectQueueFamily(queueFamilyProperties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectTransferQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                                   VkInstance vkInstance,
                                                                   VkPhysicalDevice vkPhysicalDevice) const {
  GetScoreFunctionObject getScoreFuncObj = GetTransferScore;
  return SelectQueueFamily(queueFamilyProperties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> SelectQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                     const GetScoreFunctionObject& getScoreFunctionObject,
                                     VkInstance vkInstance,
                                     VkPhysicalDevice vkPhysicalDevice) {
  std::multimap<FQueueFamilyScore, u32> ratings;
  std::ranges::for_each(queueFamilyProperties,
                        [&ratings, getScoreFunctionObject, &vkInstance, &vkPhysicalDevice, idx = 0](const VkQueueFamilyProperties& properties) mutable{
                          FQueueFamilyScore score = getScoreFunctionObject(properties, idx, vkInstance, vkPhysicalDevice);
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
                                   VkInstance vkInstance,
                                   VkPhysicalDevice vkPhysicalDevice) {
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
    score += 100;
  }
  return score;
}


FQueueFamilyScore GetPresentScore(VkQueueFamilyProperties properties,
                                  u32 queueFamilyIndex,
                                  VkInstance vkInstance,
                                  VkPhysicalDevice vkPhysicalDevice) {
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
    score += 50;
  }

#ifdef WIN32
  auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
      (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
          vkInstance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
  VkBool32 supported = vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, queueFamilyIndex);
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
                                   VkInstance vkInstance,
                                   VkPhysicalDevice vkPhysicalDevice) {
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
