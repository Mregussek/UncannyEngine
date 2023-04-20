
#ifndef UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H


#include "AccelerationStructure.h"
#include <span>


namespace uncanny::vulkan
{


class FBottomLevelAccelerationStructure final : public FAccelerationStructure
{
public:

  FBottomLevelAccelerationStructure() = default;
  FBottomLevelAccelerationStructure(VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Build(std::span<FRenderVertex> vertices, std::span<u32> indices, math::Matrix4x4f transform,
             const FCommandPool& commandPool, const FQueue& queue);

  void Build(std::span<FRenderVertex> vertices, std::span<u32> indices, const FCommandPool& commandPool,
             const FQueue& queue);

  void AssignTransformMatrix(math::Matrix4x4f transform);

  [[nodiscard]] const VkTransformMatrixKHR& GetTransform() const { return m_Transform; }

private:

  VkTransformMatrixKHR m_Transform{};

};


}


#endif //UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
