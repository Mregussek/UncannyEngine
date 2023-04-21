
#include "BottomLevelAccelerationStructure.h"


namespace uncanny::vulkan
{


FBottomLevelAccelerationStructure::FBottomLevelAccelerationStructure(
    VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes)
    : FAccelerationStructure(vkDevice, pPhysicalDeviceAttributes),
      m_VertexBuffer(pPhysicalDeviceAttributes, vkDevice),
      m_IndexBuffer(pPhysicalDeviceAttributes, vkDevice)
{
  m_Transform = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f
  };
}


void FBottomLevelAccelerationStructure::Destroy()
{
  FAccelerationStructure::Destroy();
  m_VertexBuffer.Free();
  m_IndexBuffer.Free();
}


void FBottomLevelAccelerationStructure::Build(std::span<FRenderVertex> vertices, std::span<u32> indices,
                                             math::Matrix4x4f transform, const FCommandPool& commandPool,
                                             const FQueue& queue)
{
  AssignTransformMatrix(transform);
  Build(vertices, indices, commandPool, queue);
}


void FBottomLevelAccelerationStructure::Build(std::span<FRenderVertex> vertices, std::span<u32> indices,
                                              const FCommandPool& commandPool, const FQueue& queue)
{
  VkBufferUsageFlags bufferUsageFlags =
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
      VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  m_VertexBuffer.Allocate(vertices.size() * sizeof(FRenderVertex),
                          bufferUsageFlags | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_VertexBuffer.FillStaged(vertices.data(), sizeof(FRenderVertex), vertices.size(), commandPool, queue);

  m_IndexBuffer.Allocate(indices.size() * sizeof(u32),
                         bufferUsageFlags | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_IndexBuffer.FillStaged(indices.data(), sizeof(u32), indices.size(), commandPool, queue);

  u32 trianglesCount = m_IndexBuffer.GetFilledElementsCount() / 3;
  u32 vertexMaxCount = m_VertexBuffer.GetFilledElementsCount();

  VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{
      .deviceAddress = m_VertexBuffer.GetDeviceAddress()
  };
  VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{
      .deviceAddress = m_IndexBuffer.GetDeviceAddress()
  };

  VkAccelerationStructureGeometryKHR geometryInfo{
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
      .pNext = nullptr,
      .geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR,
      .geometry = {
          .triangles = {
              .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
              .pNext = nullptr,
              .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
              .vertexData = vertexBufferDeviceAddress,
              .vertexStride = m_VertexBuffer.GetFilledStride(),
              .maxVertex = vertexMaxCount,
              .indexType = VK_INDEX_TYPE_UINT32,
              .indexData = indexBufferDeviceAddress,
              .transformData = {}
          }
      },
      .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
  };

  FAccelerationStructure::AcquireSizeForBuild(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, trianglesCount,
                                              &geometryInfo);
  FAccelerationStructure::Create(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);
  FAccelerationStructure::Build(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, &geometryInfo, trianglesCount,
                                commandPool, queue);
}


void FBottomLevelAccelerationStructure::AssignTransformMatrix(math::Matrix4x4f transform)
{
  m_Transform = {
      transform[0], transform[4], transform[8], transform[12],
      transform[1], transform[5], transform[9], transform[13],
      transform[2], transform[6], transform[10], transform[14]
  };
}


}
