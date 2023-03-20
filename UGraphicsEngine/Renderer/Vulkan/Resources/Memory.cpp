
#include "Memory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


struct FMemoryTypeIndexResult
{
  u32 index{ UUNUSED };
  b8 found{ UFALSE };
};


static FMemoryTypeIndexResult FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties properties, u32 typeFilter,
                                                  VkMemoryPropertyFlags flags);


void FMemory::Allocate(VkDevice vkDevice, VkPhysicalDeviceMemoryProperties memoryProperties,
                       VkMemoryRequirements requirements, VkMemoryPropertyFlags memoryFlags)
{
  m_Device = vkDevice;

  FMemoryTypeIndexResult memoryTypeIndexResult = FindMemoryTypeIndex(memoryProperties, requirements.memoryTypeBits,
                                                                     memoryFlags);
  if (not memoryTypeIndexResult.found)
  {
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "No memory type index!");
  }

  VkMemoryAllocateInfo memoryAllocateInfo{
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = nullptr,
    .allocationSize = requirements.size,
    .memoryTypeIndex = memoryTypeIndexResult.index
  };

  VkResult result = vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &m_DeviceMemory);
  AssertVkAndThrow(result);
}


void FMemory::Free()
{
  if (m_DeviceMemory != VK_NULL_HANDLE)
  {
    vkFreeMemory(m_Device, m_DeviceMemory, nullptr);
  }
}


FMemoryTypeIndexResult FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties properties, u32 typeFilter,
                                           VkMemoryPropertyFlags flags)
{
  for (u32 i = 0; i < properties.memoryTypeCount; i++) {
    // Check each memory type to see if its bit is set to 1.
    if (typeFilter & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
      return { .index = i, .found = UTRUE };
    }
  }
  return { .index = UUNUSED, .found = UFALSE };
}


}