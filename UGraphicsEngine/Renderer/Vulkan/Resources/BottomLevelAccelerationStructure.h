
#ifndef UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_BOTTOMLEVELACCELERATIONSTRUCTURE_H


#include "AccelerationStructure.h"
#include <span>


namespace uncanny::vulkan
{


class FBottomLevelAccelerationStructure : public FAccelerationStructure
{
public:

  ~FBottomLevelAccelerationStructure();

  void Build(const FRenderMeshData& meshData, std::span<FRenderMaterialData> materials, const FCommandPool& commandPool,
             const FQueue& queue, VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void Destroy();

  [[nodiscard]] const VkTransformMatrixKHR& GetTransform() const { return m_Transform; }
  [[nodiscard]] const FBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
  [[nodiscard]] const FBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
  [[nodiscard]] const FBuffer& GetMaterialBuffer() const { return m_MaterialBuffer; }
  [[nodiscard]] const FBuffer& GetMaterialIndicesBuffer() const { return m_MaterialIndexBuffer; }

private:

  void AssignTransformMatrix(math::Matrix4x4f transform);

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
