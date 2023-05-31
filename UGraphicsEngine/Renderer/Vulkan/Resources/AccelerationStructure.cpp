
#include "AccelerationStructure.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"


namespace uncanny::vulkan
{


void FAccelerationStructure::AcquireSizeForBuild(VkAccelerationStructureTypeKHR type, u32 trianglesCount,
                                                 const VkAccelerationStructureGeometryKHR* pGeometry)
{
  VkAccelerationStructureBuildGeometryInfoKHR buildSizeGeometryInfo{
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
      .type = type,
      .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
      .geometryCount = 1,
      .pGeometries = pGeometry,
  };
  VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo{
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR
  };
  vkGetAccelerationStructureBuildSizesKHR(m_Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
                                          &buildSizeGeometryInfo, &trianglesCount, &buildSizesInfo);
  m_Size = buildSizesInfo.accelerationStructureSize;
  m_ScratchSize = buildSizesInfo.buildScratchSize;
}


void FAccelerationStructure::Create(VkAccelerationStructureTypeKHR type)
{
  VkBufferUsageFlags accelerationUsageFlags =
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  m_AccelerationMemoryBuffer.Allocate(m_Size, accelerationUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      m_Device, m_pPhysicalDeviceAttributes);

  VkAccelerationStructureCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
      .pNext = nullptr,
      .createFlags = 0,
      .buffer = m_AccelerationMemoryBuffer.GetHandle(),
      .offset = 0,
      .size = m_Size,
      .type = type,
      .deviceAddress = {}
  };

  VkResult result = vkCreateAccelerationStructureKHR(m_Device, &createInfo, nullptr, &m_AccelerationStructure);
  AssertVkAndThrow(result);

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


void FAccelerationStructure::Build(VkAccelerationStructureTypeKHR type,
                                   const VkAccelerationStructureGeometryKHR* pGeometry,
                                   u32 primitiveCount, const FCommandPool& commandPool, const FQueue& queue)
{
  FBuffer scratchBuffer{};
  scratchBuffer.Allocate(m_ScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Device, m_pPhysicalDeviceAttributes);
  VkDeviceOrHostAddressKHR scratchDeviceAddress{
      .deviceAddress = scratchBuffer.GetDeviceAddress()
  };

  VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR,
      .pNext = nullptr,
      .type = type,
      .flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR,
      .mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
      .srcAccelerationStructure = VK_NULL_HANDLE,
      .dstAccelerationStructure = m_AccelerationStructure,
      .geometryCount = 1,
      .pGeometries = pGeometry,
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

  VkAccessFlags accessFlags = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
                              VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
  VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

  FCommandBuffer commandBuffer = commandPool.AllocatePrimaryCommandBuffer();
  commandBuffer.BeginOneTimeRecording();
  commandBuffer.MemoryBarrier(accessFlags, accessFlags, stageFlags, stageFlags);
  commandBuffer.BuildAccelerationStructure(&buildGeometryInfo, buildRangeInfos);
  commandBuffer.EndRecording();

  queue.Submit({}, {}, commandBuffer, {}, VK_NULL_HANDLE);
  queue.WaitIdle();

  vkDeviceWaitIdle(m_Device);
}


void FAccelerationStructure::Destroy()
{
  if (m_AccelerationStructure != VK_NULL_HANDLE)
  {
    vkDestroyAccelerationStructureKHR(m_Device, m_AccelerationStructure, nullptr);
    m_AccelerationStructure = VK_NULL_HANDLE;
  }

  m_AccelerationMemoryBuffer.Free();
}


}
