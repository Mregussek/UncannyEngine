
#include "BottomLevelAS.h"
#include "UGraphicsEngine/Renderer/RenderMesh.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderDeviceFactory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Fence.h"


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
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
      VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  FBuffer vertexBuffer{};
  FBuffer indexBuffer{};

  vertexBuffer = m_pRenderDeviceFactory->CreateBuffer();
  vertexBuffer.Allocate(vertices.size() * sizeof(FVertex), bufferUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vertexBuffer.FillStaged(vertices.data(), sizeof(FVertex), vertices.size(), commandPool, queue);

  indexBuffer = m_pRenderDeviceFactory->CreateBuffer();
  indexBuffer.Allocate(indices.size() * sizeof(u32), bufferUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  indexBuffer.FillStaged(indices.data(), sizeof(u32), indices.size(), commandPool, queue);

  VkDeviceOrHostAddressConstKHR vertexBufferDeviceAddress{
    .deviceAddress = vertexBuffer.GetDeviceAddress()
  };
  VkDeviceOrHostAddressConstKHR indexBufferDeviceAddress{
    .deviceAddress = indexBuffer.GetDeviceAddress()
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
          .vertexStride = vertexBuffer.GetFilledStride(),
          .maxVertex = vertexBuffer.GetFilledElementsCount(),
          .indexType = VK_INDEX_TYPE_UINT32,
          .indexData = indexBufferDeviceAddress,
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
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR,
    .pNext = nullptr,
    .accelerationStructureSize = 0,
    .updateScratchSize = 0,
    .buildScratchSize = 0
  };
  u32 primitiveCount = indices.size() / 3;
  vkGetAccelerationStructureBuildSizesKHR(m_Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                          &buildSizeGeometryInfo, &primitiveCount, &buildSizesInfo);

  // reserve memory to hold the acceleration structure
  VkBufferUsageFlags accelerationUsageFlags =
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  m_AccelerationMemoryBuffer = m_pRenderDeviceFactory->CreateBuffer();
  m_AccelerationMemoryBuffer.Allocate(buildSizesInfo.accelerationStructureSize, accelerationUsageFlags,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkAccelerationStructureCreateInfoKHR createInfo{
    .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
    .pNext = nullptr,
    .createFlags = 0,
    .buffer = m_AccelerationMemoryBuffer.GetHandle(),
    .offset = 0,
    .size = buildSizesInfo.accelerationStructureSize,
    .type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
    .deviceAddress = {}
  };
  VkResult result = vkCreateAccelerationStructureKHR(m_Device, &createInfo, nullptr, &m_AccelerationStructure);
  AssertVkAndThrow(result);

  // reserve memory to build acceleration structure
  VkBufferUsageFlags accelerationBuildUsageFlags =
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  FBuffer scratchBuffer = m_pRenderDeviceFactory->CreateBuffer();
  scratchBuffer.Allocate(buildSizesInfo.accelerationStructureSize, accelerationBuildUsageFlags,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VkDeviceOrHostAddressKHR scratchDeviceAddress{
      .deviceAddress = scratchBuffer.GetDeviceAddress()
  };

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
    .scratchData = scratchDeviceAddress
  };

  VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo{
    .primitiveCount = primitiveCount,
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0
  };
  VkAccelerationStructureBuildRangeInfoKHR* buildRangeInfos[]{ &buildRangeInfo };

  FCommandBuffer commandBuffer = commandPool.AllocatePrimaryCommandBuffer();
  commandBuffer.BeginOneTimeRecording();
  commandBuffer.BuildAccelerationStructure(&buildGeometryInfo, buildRangeInfos);
  commandBuffer.EndRecording();

  FFence fence{};
  fence.Create(m_Device, 0);

  queue.Submit({}, {}, commandBuffer, {}, fence.GetHandle());

  fence.WaitAndReset();

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

  m_AccelerationMemoryBuffer.Free();
}


}
