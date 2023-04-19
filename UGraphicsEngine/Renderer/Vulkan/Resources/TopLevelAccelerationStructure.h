
#ifndef UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H


#include "BottomLevelAccelerationStructure.h"


namespace uncanny::vulkan
{


class FTopLevelAccelerationStructure : public FAccelerationStructure
{
public:

  FTopLevelAccelerationStructure() = default;
  FTopLevelAccelerationStructure(VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Build(const FBottomLevelAccelerationStructure& bottomLevelAS, const FCommandPool& commandPool,
             const FQueue& queue);

};


}


#endif //UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
