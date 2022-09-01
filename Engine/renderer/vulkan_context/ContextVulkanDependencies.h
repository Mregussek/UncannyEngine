
#ifndef UNCANNYENGINE_CONTEXTVULKANDEPENDENCIES_H
#define UNCANNYENGINE_CONTEXTVULKANDEPENDENCIES_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


enum class EQueueFamilyMainUsage {
  NONE, GRAPHICS, TRANSFER
};


enum class EPhysicalDeviceType {
  NONE, INTEGRATED, DISCRETE
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


b32 getQueueFamilyDependencies(
    EQueueFamilyMainUsage mainUsage,
    const std::vector<FQueueFamilyDependencies>& queueFamilyDependenciesVector,
    FQueueFamilyDependencies* pOutput);


}


#endif //UNCANNYENGINE_CONTEXTVULKANDEPENDENCIES_H
