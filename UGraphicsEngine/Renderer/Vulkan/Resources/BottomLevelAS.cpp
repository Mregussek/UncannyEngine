
#include <array>
#include "BottomLevelAS.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Mesh.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderDeviceFactory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FBottomLevelAS::FBottomLevelAS(const FRenderDeviceFactory* pFactory, VkDevice vkDevice)
  : m_pRenderDeviceFactory(pFactory),
  m_Device(vkDevice)
{
}


void FBottomLevelAS::Build(std::span<FVertex> vertices, std::span<u32> indices, const FCommandPool& commandPool,
                           const FQueue& queue)
{
  VkBufferUsageFlags bufferUsageFlags =
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

  FBuffer vertexBuffer = m_pRenderDeviceFactory->CreateBuffer();
  vertexBuffer.Allocate(vertices.size() * sizeof(vulkan::FVertex), bufferUsageFlags,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  vertexBuffer.Fill(vertices.data(), sizeof(vulkan::FVertex), vertices.size());

  FBuffer indexBuffer = m_pRenderDeviceFactory->CreateBuffer();
  indexBuffer.Allocate(vertices.size() * sizeof(u32), bufferUsageFlags,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  indexBuffer.Fill(indices.data(), sizeof(u32), indices.size());

  VkAccelerationStructureGeometryKHR geometryInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
    .pNext = nullptr,
    .geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR,
    .geometry = {
      .triangles = {
          .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
          .pNext = nullptr,
          .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
          .vertexData = { .deviceAddress = vertexBuffer.GetDeviceAddress() },
          .vertexStride = vertexBuffer.GetStride(),
          .maxVertex = vertexBuffer.GetElementsCount(),
          .indexType = VK_INDEX_TYPE_UINT32,
          .indexData = { .deviceAddress = indexBuffer.GetDeviceAddress() },
          .transformData = {}
      }
    },
    .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
  };

  VkAccelerationStructureBuildGeometryInfoKHR buildSizeGeometryInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = nullptr,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
    .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
    .srcAccelerationStructure = VK_NULL_HANDLE,
    .dstAccelerationStructure = VK_NULL_HANDLE,
    .geometryCount = 1,
    .pGeometries = &geometryInfo,
    .ppGeometries = nullptr,
    .scratchData = {}
  };

  // acquire size to build acceleration structure
  VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR
  };
  u32 primitiveCount = vertexBuffer.GetElementsCount();
  vkGetAccelerationStructureBuildSizesKHR(m_Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                          &buildSizeGeometryInfo, &primitiveCount, &buildSizesInfo);

  // reserve memory to hold the acceleration structure
  VkBufferUsageFlags accelerationUsageFlags =
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  FBuffer accelerationMemoryBuffer = m_pRenderDeviceFactory->CreateBuffer();
  accelerationMemoryBuffer.Allocate(buildSizesInfo.accelerationStructureSize, accelerationUsageFlags,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkAccelerationStructureCreateInfoKHR createInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .pNext = nullptr,
    .createFlags = 0,
    .buffer = accelerationMemoryBuffer.GetHandle(),
    .offset = 0,
    .size = buildSizesInfo.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .deviceAddress = {}
  };
  VkResult result = vkCreateAccelerationStructureKHR(m_Device, &createInfo, nullptr, &m_AccelerationStructure);
  AssertVkAndThrow(result);

  // reserve memory to build acceleration structure
  VkBufferUsageFlags accelerationBuildUsageFlags =
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  FBuffer accelerationBuildMemoryBuffer = m_pRenderDeviceFactory->CreateBuffer();
  accelerationBuildMemoryBuffer.Allocate(buildSizesInfo.accelerationStructureSize, accelerationBuildUsageFlags,
                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
    .pNext = nullptr,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
    .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
    .srcAccelerationStructure = VK_NULL_HANDLE,
    .dstAccelerationStructure = m_AccelerationStructure,
    .geometryCount = 1,
    .pGeometries = &geometryInfo,
    .ppGeometries = nullptr,
    .scratchData = { .deviceAddress = accelerationBuildMemoryBuffer.GetDeviceAddress() }
  };

  VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo{
    .primitiveCount = primitiveCount,
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0
  };
  std::array<VkAccelerationStructureBuildRangeInfoKHR*, 1> buildRangeInfos{ &buildRangeInfo };

  FCommandBuffer commandBuffer = commandPool.AllocatePrimaryCommandBuffer();
  commandBuffer.BeginOneTimeRecording();
  commandBuffer.BuildAccelerationStructure(&buildGeometryInfo, buildRangeInfos.data());
  commandBuffer.EndRecording();

  queue.Submit({}, {}, commandBuffer, {}, VK_NULL_HANDLE);
  queue.WaitIdle();

  VkAccelerationStructureDeviceAddressInfoKHR addressInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
    .pNext = nullptr,
    .accelerationStructure = m_AccelerationStructure
  };
  m_DeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(m_Device, &addressInfo);

  if (m_DeviceAddress == 0)
  {
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Invalid device address to bottom AS!");
  }
}


void FBottomLevelAS::Destroy()
{
  if (m_AccelerationStructure != VK_NULL_HANDLE)
  {
    vkDestroyAccelerationStructureKHR(m_Device, m_AccelerationStructure, nullptr);
  }
}


}
