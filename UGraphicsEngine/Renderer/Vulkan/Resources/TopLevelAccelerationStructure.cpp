
#include "TopLevelAccelerationStructure.h"


namespace uncanny::vulkan
{


void FTopLevelAccelerationStructure::Build(const FBottomLevelAccelerationStructure& bottomLevelStructure,
                                           const FCommandPool& commandPool, const FQueue& queue, VkDevice vkDevice,
                                           const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes)
{
  std::span<const FBottomLevelAccelerationStructure> span(&bottomLevelStructure, 1);
  Build(span, commandPool, queue, vkDevice, pPhysicalDeviceAttributes);
}


void FTopLevelAccelerationStructure::Build(std::span<const FBottomLevelAccelerationStructure> bottomLevelStructures,
                                           const FCommandPool& commandPool, const FQueue& queue, VkDevice vkDevice,
                                           const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes)
{
  m_Device = vkDevice;
  m_pPhysicalDeviceAttributes = pPhysicalDeviceAttributes;

  std::vector<VkAccelerationStructureInstanceKHR> instances;
  instances.reserve(bottomLevelStructures.size());
  m_BottomUniformData.reserve(bottomLevelStructures.size());
  for (u32 i = 0; i < bottomLevelStructures.size(); i++)
  {
    instances.push_back({
      .transform = bottomLevelStructures[i].GetTransform(),
      .instanceCustomIndex = i,
      .mask = 0xFF,
      .instanceShaderBindingTableRecordOffset = 0,
      .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
      .accelerationStructureReference = bottomLevelStructures[i].GetDeviceAddress()
    });
    m_BottomUniformData.push_back(FBottomLevelStructureReferenceUniformData{
      .vertexBufferDeviceAddress = bottomLevelStructures[i].GetVertexBuffer().GetDeviceAddress(),
      .indexBufferDeviceAddress = bottomLevelStructures[i].GetIndexBuffer().GetDeviceAddress(),
      .materialBufferDeviceAddress = bottomLevelStructures[i].GetMaterialBuffer().GetDeviceAddress(),
      .materialIndicesBufferDeviceAddress = bottomLevelStructures[i].GetMaterialIndicesBuffer().GetDeviceAddress(),
    });
  }
  Build(instances, commandPool, queue);
}


void FTopLevelAccelerationStructure::Build(std::span<VkAccelerationStructureInstanceKHR> instances,
                                           const FCommandPool&commandPool, const FQueue& queue)
{
  u32 instancesCount = instances.size();

  FBuffer instanceBuffer{};
  VkBufferUsageFlags usageFlags =
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
      VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  instanceBuffer.Allocate(sizeof(VkAccelerationStructureInstanceKHR) * instancesCount, usageFlags,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Device, m_pPhysicalDeviceAttributes);
  instanceBuffer.FillStaged(instances.data(), sizeof(VkAccelerationStructureInstanceKHR), instancesCount, commandPool,
                            queue);
  VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{
      .deviceAddress = instanceBuffer.GetDeviceAddress()
  };

  VkAccelerationStructureGeometryKHR geometryInfo{
      .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR,
      .pNext = nullptr,
      .geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR,
      .geometry = {
          .instances = {
              .sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR,
              .pNext = nullptr,
              .arrayOfPointers = VK_FALSE,
              .data = instanceDataDeviceAddress
          }
      },
      .flags = VK_GEOMETRY_OPAQUE_BIT_KHR
  };

  FAccelerationStructure::AcquireSizeForBuild(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, instancesCount,
                                              &geometryInfo);
  FAccelerationStructure::Create(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);
  FAccelerationStructure::Build(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, &geometryInfo, instancesCount,
                                commandPool, queue);
}


}
