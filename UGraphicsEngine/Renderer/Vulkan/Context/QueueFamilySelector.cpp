
#include "QueueFamilySelector.h"
#include <algorithm>
#include <map>
#include <functional>


namespace uncanny::vulkan
{


typedef i32 FQueueFamilyScore;
typedef std::function<FQueueFamilyScore(VkQueueFamilyProperties, u32, VkInstance, VkPhysicalDevice)> GetScoreFuncObject;


static std::optional<u32> SelectQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                            const GetScoreFuncObject& getScoreFunctionObject,
                                            VkInstance vkInstance,
                                            VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetGraphicsScore(VkQueueFamilyProperties properties, u32 queueFamilyIndex,
                                          VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetPresentScore(VkQueueFamilyProperties properties, u32 queueFamilyIndex,
                                         VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetTransferScore(VkQueueFamilyProperties properties, u32 queueFamilyIndex,
                                          VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);


static FQueueFamilyScore GetComputeScore(VkQueueFamilyProperties properties, u32 queueFamilyIndex,
                                         VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);


std::optional<u32> FQueueFamilySelector::SelectGraphicsQueueFamily(std::span<const VkQueueFamilyProperties> properties,
                                                                   VkInstance vkInstance,
                                                                   VkPhysicalDevice vkPhysicalDevice) const
{
  GetScoreFuncObject getScoreFuncObj = GetGraphicsScore;
  return SelectQueueFamily(properties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectPresentQueueFamily(std::span<const VkQueueFamilyProperties> properties,
                                                                  VkInstance vkInstance,
                                                                  VkPhysicalDevice vkPhysicalDevice) const
{
  GetScoreFuncObject getScoreFuncObj = GetPresentScore;
  return SelectQueueFamily(properties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectTransferQueueFamily(std::span<const VkQueueFamilyProperties> properties,
                                                                   VkInstance vkInstance,
                                                                   VkPhysicalDevice vkPhysicalDevice) const
{
  GetScoreFuncObject getScoreFuncObj = GetTransferScore;
  return SelectQueueFamily(properties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> FQueueFamilySelector::SelectComputeQueueFamily(std::span<const VkQueueFamilyProperties> properties,
                                                                  VkInstance vkInstance,
                                                                  VkPhysicalDevice vkPhysicalDevice) const
{
  GetScoreFuncObject getScoreFuncObj = GetComputeScore;
  return SelectQueueFamily(properties, getScoreFuncObj, vkInstance, vkPhysicalDevice);
}


std::optional<u32> SelectQueueFamily(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                     const GetScoreFuncObject& getScoreFunctionObject,
                                     VkInstance vkInstance,
                                     VkPhysicalDevice vkPhysicalDevice)
{
  std::multimap<FQueueFamilyScore, u32> ratings;
  std::ranges::for_each(queueFamilyProperties,
                        [&, getScoreFunctionObject, idx = 0](const VkQueueFamilyProperties& properties) mutable
  {
    FQueueFamilyScore score = getScoreFunctionObject(properties, idx, vkInstance, vkPhysicalDevice);
    ratings.insert(std::make_pair(score, idx));
    ++idx;
  });
  auto highestRatingIt = ratings.rbegin();
  if (highestRatingIt->first == 0)
  {
    return std::nullopt;
  }
  return highestRatingIt->second;
}


FQueueFamilyScore GetGraphicsScore(VkQueueFamilyProperties properties,
                                   u32 queueFamilyIndex,
                                   VkInstance vkInstance,
                                   VkPhysicalDevice vkPhysicalDevice)
{
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
  {
    score += 100;
  }
  return score;
}


FQueueFamilyScore GetPresentScore(VkQueueFamilyProperties properties,
                                  u32 queueFamilyIndex,
                                  VkInstance vkInstance,
                                  VkPhysicalDevice vkPhysicalDevice)
{
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
  {
    score += 50;
  }

#ifdef WIN32
  auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
      (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
          vkInstance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
  VkBool32 supported = vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, queueFamilyIndex);
  if (supported)
  {
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
                                   VkPhysicalDevice vkPhysicalDevice)
{
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
  {
    score -= 25;
  }
  if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
  {
    score -= 25;
  }
  if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
  {
    score += 100;
  }
  return score;
}


FQueueFamilyScore GetComputeScore(VkQueueFamilyProperties properties,
                                  u32 queueFamilyIndex,
                                  VkInstance vkInstance,
                                  VkPhysicalDevice vkPhysicalDevice)
{
  FQueueFamilyScore score{ 0 };
  if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
  {
    score += 100;
  }
  if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
  {
    score -= 25;
  }
  if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
  {
    score -= 25;
  }
  return score;
}


}
