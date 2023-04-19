
#include "TopLevelAccelerationStructure.h"


namespace uncanny::vulkan
{


FTopLevelAccelerationStructure::FTopLevelAccelerationStructure(
    VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes)
    : FAccelerationStructure(vkDevice, pPhysicalDeviceAttributes)
{

}


void FTopLevelAccelerationStructure::Build(const FBottomLevelAccelerationStructure& bottomLevelAS,
                                           const FCommandPool&commandPool, const FQueue& queue)
{
  VkTransformMatrixKHR instanceTransform{
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f
  };

  VkAccelerationStructureInstanceKHR instance{
      .transform = instanceTransform,
      .instanceCustomIndex = 0,
      .mask = 0xFF,
      .instanceShaderBindingTableRecordOffset = 0,
      .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
      .accelerationStructureReference = bottomLevelAS.GetDeviceAddress()
  };

  FBuffer instanceBuffer(m_pPhysicalDeviceAttributes, m_Device);
  VkBufferUsageFlags usageFlags =
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
  instanceBuffer.Allocate(sizeof(VkAccelerationStructureInstanceKHR), usageFlags,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  instanceBuffer.Fill(&instance, sizeof(VkAccelerationStructureInstanceKHR), 1);
  VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress{
      .deviceAddress = instanceBuffer.GetDeviceAddress()
  };

  u32 instancesCount = instanceBuffer.GetFilledElementsCount();

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
