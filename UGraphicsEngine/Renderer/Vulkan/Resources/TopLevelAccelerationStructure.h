
#ifndef UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H


#include "BottomLevelAccelerationStructure.h"
#include <span>


namespace uncanny::vulkan
{


struct FBottomLevelStructureReferenceUniformData
{
  u64 vertexBufferDeviceAddress{ UUNUSED };
  u64 indexBufferDeviceAddress{ UUNUSED };
  u64 materialBufferDeviceAddress{ UUNUSED };
  u64 materialIndicesBufferDeviceAddress{ UUNUSED };
};


class FTopLevelAccelerationStructure final : public FAccelerationStructure
{
public:

  void Build(const FBottomLevelAccelerationStructure& bottomLevelStructure, const FCommandPool& commandPool,
             const FQueue& queue, VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Build(std::span<const FBottomLevelAccelerationStructure> bottomLevelStructures, const FCommandPool& commandPool,
             const FQueue& queue, VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  [[nodiscard]] const std::vector<FBottomLevelStructureReferenceUniformData>& GetBLASReferenceUniformData() const
  {
    return m_BottomUniformData;
  }

private:

  void Build(std::span<VkAccelerationStructureInstanceKHR> instances, const FCommandPool& commandPool,
             const FQueue& queue);

private:

  std::vector<FBottomLevelStructureReferenceUniformData> m_BottomUniformData{};

};


}


#endif //UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
