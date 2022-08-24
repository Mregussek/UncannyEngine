
#ifndef UNCANNYENGINE_RENDERCONTEXTDEPENDENCIES_H
#define UNCANNYENGINE_RENDERCONTEXTDEPENDENCIES_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


enum class EQueueFamilyMainUsage {
  NONE, GRAPHICS
};


enum class EPhysicalDeviceType {
  NONE, INTEGRATED, DISCRETE
};


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


struct FWindowSurfaceDependencies {
  std::vector<VkPresentModeKHR> presentModeCandidates{};
};


struct FQueueFamilyDependencies {
  // @brief queues priorities for given count
  std::vector<f32> queuesPriorities{};
  // @brief main usage for queue family
  EQueueFamilyMainUsage mainUsage{ EQueueFamilyMainUsage::NONE };
  // @brief how many queues of such type are needed
  u32 queuesCountNeeded{ 0 };
  // @brief support graphics operations
  u32 graphics{ UFALSE };
  // @brief support compute operations
  u32 compute{ UFALSE };
  // @brief support transfer operations
  u32 transfer{ UFALSE };
  // @brief support sparse memory management operations
  u32 sparseBinding{ UFALSE };
};


struct FPhysicalDeviceDependencies {
  // @brief what type of device do you prefer -> integrated / discrete
  EPhysicalDeviceType deviceType{ EPhysicalDeviceType::NONE };
  // @brief if above is not available, what fallback device is preferred
  EPhysicalDeviceType deviceTypeFallback{ EPhysicalDeviceType::NONE };
  // @brief how many queue family indexes types are needed (for graphics, for transfer etc.)
  u32 queueFamilyIndexesCount{ 0 };
  // @brief dependencies for queue families. Count should be equal to queueFamilyIndexesCount
  std::vector<FQueueFamilyDependencies> queueFamilyDependencies{};
};


struct FInstanceDependencies {
  // @brief vulkan api version expected
  u32 vulkanApiVersion{ 0 };
};


// @brief Implementation at VulkanPhysicalDevice.cpp
FQueueFamilyDependencies getQueueFamilyDependencies(
    EQueueFamilyMainUsage mainUsage,
    const std::vector<FQueueFamilyDependencies>& queueFamilyDependenciesVector);


}


#endif //UNCANNYENGINE_RENDERCONTEXTDEPENDENCIES_H
