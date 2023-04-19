
#ifndef UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H


#include "AccelerationStructure.h"
#include <span>


namespace uncanny::vulkan
{


class FBottomLevelAccelerationStructure : public FAccelerationStructure
{
public:

  FBottomLevelAccelerationStructure() = default;
  FBottomLevelAccelerationStructure(VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Build(std::span<FVertex> vertices, std::span<u32> indices, const FCommandPool& commandPool,
             const FQueue& queue);

};


}


#endif //UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
