
#ifndef UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H


#include "BottomLevelAccelerationStructure.h"
#include <span>


namespace uncanny::vulkan
{


class FTopLevelAccelerationStructure final : public FAccelerationStructure
{
public:

  FTopLevelAccelerationStructure() = default;
  FTopLevelAccelerationStructure(VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Build(const FBottomLevelAccelerationStructure& bottomLevelStructure, const FCommandPool& commandPool,
             const FQueue& queue);

  void Build(std::span<FBottomLevelAccelerationStructure> bottomLevelStructures, const FCommandPool& commandPool,
             const FQueue& queue);

  void Build(std::span<VkAccelerationStructureInstanceKHR> instances, const FCommandPool& commandPool,
             const FQueue& queue);

};


}


#endif //UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
