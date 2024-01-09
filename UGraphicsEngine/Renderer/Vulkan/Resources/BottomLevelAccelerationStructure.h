
#ifndef UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H


#include "AccelerationStructure.h"
#include <span>


namespace uncanny::vulkan
{


/// @brief FBottomLevelAccelerationStructure is a wrapper class for VkAccelerationStructureKHR that represents BLAS.
class FBottomLevelAccelerationStructure : public FAccelerationStructure
{
public:

  FBottomLevelAccelerationStructure() = default;

  FBottomLevelAccelerationStructure(const FBottomLevelAccelerationStructure& other) = delete;
  FBottomLevelAccelerationStructure(FBottomLevelAccelerationStructure&& other) = default;

  FBottomLevelAccelerationStructure& operator=(const FBottomLevelAccelerationStructure& other) = delete;
  FBottomLevelAccelerationStructure& operator=(FBottomLevelAccelerationStructure&& other) = delete;

  ~FBottomLevelAccelerationStructure();

public:

  /// @brief In general builds BLAS from mesh data and materials.
  /// Allocates vertex, index, material and material index buffers from mesh data and materials. For this operation
  /// it uses given command pool and queue. After that it builds BLAS.
  void Build(const FRenderMeshData& meshData, std::span<FRenderMaterialData> materials, const FCommandPool& commandPool,
             const FQueue& queue, VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Destroy();

// Getters
public:

  [[nodiscard]] const VkTransformMatrixKHR& GetTransform() const { return m_Transform; }
  [[nodiscard]] const FBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
  [[nodiscard]] const FBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
  [[nodiscard]] const FBuffer& GetMaterialBuffer() const { return m_MaterialBuffer; }
  [[nodiscard]] const FBuffer& GetMaterialIndicesBuffer() const { return m_MaterialIndexBuffer; }

// Private methods
private:

  void AssignTransformMatrix(math::Matrix4x4f transform);

// Members
private:

  FBuffer m_VertexBuffer{};
  FBuffer m_IndexBuffer{};
  FBuffer m_MaterialBuffer{};
  FBuffer m_MaterialIndexBuffer{};
  VkTransformMatrixKHR m_Transform{
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f
  };

};


}


#endif //UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
