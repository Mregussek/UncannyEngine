
#include "PhysicalDeviceSelector.h"
#include <algorithm>
#include <map>
#include "PhysicalDeviceAttributes.h"


namespace uncanny::vulkan
{


typedef i32 FPhysicalDeviceScore;


FPhysicalDeviceScore GetScore(VkPhysicalDevice physicalDevice)
{
  FPhysicalDeviceScore score{ 0 };

  FPhysicalDeviceAttributes attributes{};
  attributes.Initialize(physicalDevice);

  switch (attributes.GetDeviceProperties().deviceType)
  {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      score += 1000;
      break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      score += 10000;
      break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      score += 100;
      break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      score += 10;
      break;
    default:
      break;
  }

  if (attributes.IsExtensionPresent(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
  {
    score += 100;
  }

  return score;
}


VkPhysicalDevice FPhysicalDeviceSelector::Select(std::span<VkPhysicalDevice> availablePhysicalDevices) const
{
  std::multimap<FPhysicalDeviceScore, VkPhysicalDevice> ratings;

  std::ranges::for_each(availablePhysicalDevices, [&ratings](VkPhysicalDevice physicalDevice)
  {
    ratings.insert(std::make_pair(GetScore(physicalDevice), physicalDevice));
  });

  return ratings.rbegin()->second;
}


}
