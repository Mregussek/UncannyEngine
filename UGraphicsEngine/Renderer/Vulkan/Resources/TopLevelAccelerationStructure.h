
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


/// @brief FTopLevelAccelerationStructure is a wrapper class for VkAccelerationStructureKHR that represents TLAS.
class FTopLevelAccelerationStructure final : public FAccelerationStructure
{
public:

  FTopLevelAccelerationStructure() = default;

  FTopLevelAccelerationStructure(const FTopLevelAccelerationStructure& other) = delete;
  FTopLevelAccelerationStructure(FTopLevelAccelerationStructure&& other) = delete;

  FTopLevelAccelerationStructure& operator=(const FTopLevelAccelerationStructure& other) = delete;
  FTopLevelAccelerationStructure& operator=(FTopLevelAccelerationStructure&& other) = delete;

  ~FTopLevelAccelerationStructure();

public:

  void Build(const FBottomLevelAccelerationStructure& bottomLevelStructure, const FCommandPool& commandPool,
             const FQueue& queue, VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Build(std::span<const FBottomLevelAccelerationStructure> bottomLevelStructures, const FCommandPool& commandPool,
             const FQueue& queue, VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Destroy();

// Getters
public:

  [[nodiscard]] const auto& GetBLASReferenceUniformData() const { return m_BottomUniformData; }

// Private methods
private:

  void Build(std::span<VkAccelerationStructureInstanceKHR> instances, const FCommandPool& commandPool,
             const FQueue& queue);

// Members
private:

  std::vector<FBottomLevelStructureReferenceUniformData> m_BottomUniformData{};

};


}


#endif //UNCANNYENGINE_TOPLEVELACCELERATIONSTRUCTURE_H
