
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


static FMemoryTypeIndexResult FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties properties, u32 typeBits,
                                                  VkMemoryPropertyFlags flags);


void FMemory::Allocate(VkDevice vkDevice, VkPhysicalDeviceMemoryProperties memoryProperties,
                       VkMemoryRequirements requirements, VkMemoryPropertyFlags memoryFlags, b8 useDeviceAddress)
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

  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .pNext = nullptr,
      .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR,
      .deviceMask = 0
  };

  if (useDeviceAddress)
  {
    memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
  }

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


FMemoryTypeIndexResult FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties memoryProperties, u32 typeBits,
                                           VkMemoryPropertyFlags flags)
{
  for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
  {
    if ((typeBits & 1) == 1)
    {
      if ((memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
      {
        return FMemoryTypeIndexResult{ i, UTRUE };
      }
    }
    typeBits >>= 1;
  }

  return FMemoryTypeIndexResult{ UUNUSED, UFALSE };
}


}