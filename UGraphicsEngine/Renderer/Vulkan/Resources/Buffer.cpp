
#include "Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/PhysicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FBuffer::~FBuffer()
{
  Free();
}


void FBuffer::Allocate(VkDeviceSize memorySize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags,
                       VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes)
{
  m_Device = vkDevice;
  m_pPhysicalDeviceAttributes = pPhysicalDeviceAttributes;
  m_MemoryPropertyFlags = memoryFlags;
  m_AllocatedMemorySize = memorySize;

  VkBufferCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .size = m_AllocatedMemorySize,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr
  };

  VkResult result = vkCreateBuffer(m_Device, &createInfo, nullptr, &m_Buffer);
  AssertVkAndThrow(result);

  VkMemoryRequirements memoryRequirements{};
  vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memoryRequirements);

  b8 useDeviceAddress{ UFALSE };
  if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
  {
    useDeviceAddress = UTRUE;
  }

  m_Memory.Allocate(m_Device, m_pPhysicalDeviceAttributes->GetMemoryProperties(), memoryRequirements,
                    m_MemoryPropertyFlags, useDeviceAddress);

  constexpr VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(m_Device, m_Buffer, m_Memory.GetHandle(), memoryOffset);

  if (useDeviceAddress)
  {
    VkBufferDeviceAddressInfoKHR addressInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext = nullptr,
        .buffer = m_Buffer
    };
    m_DeviceAddress = vkGetBufferDeviceAddressKHR(m_Device, &addressInfo);
  }
}


void FBuffer::Free()
{
  if (m_Buffer != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_Device, m_Buffer, nullptr);
    m_Buffer = VK_NULL_HANDLE;
  }
  m_Memory.Free();
}


void* FBuffer::Map()
{
  constexpr VkDeviceSize offset{ 0 };
  constexpr VkMemoryMapFlags flags{ 0 };
  void* pMapPtr{ nullptr };

  vkMapMemory(m_Device, m_Memory.GetHandle(), offset, m_AllocatedMemorySize, flags, &pMapPtr);

  return pMapPtr;
}


void FBuffer::Unmap()
{
  constexpr VkDeviceSize offset{ 0 };

  if ((m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
  {
    VkMappedMemoryRange mappedRange{
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .pNext = nullptr,
        .memory = m_Memory.GetHandle(),
        .offset = offset,
        .size = m_AllocatedMemorySize,
    };
    VkResult result = vkFlushMappedMemoryRanges(m_Device, 1, &mappedRange);
    AssertVkAndThrow(result);
  }

  vkUnmapMemory(m_Device, m_Memory.GetHandle());
}


void FBuffer::Fill(const void* pData, u32 elementSizeof, u32 elementsCount)
{
  m_Stride = elementSizeof;
  m_ElementsCount = elementsCount;
  m_ElementsSizeInBytes = m_Stride * m_ElementsCount;

  void* pMapPtr = Map();
  memcpy(pMapPtr, pData, m_ElementsSizeInBytes);
  Unmap();
}


void FBuffer::FillStaged(const void* pData, u32 elementSizeof, u32 elementsCount,
                         const FCommandPool& transferCommandPool, const FQueue& transferQueue)
{
  m_Stride = elementSizeof;
  m_ElementsCount = elementsCount;
  m_ElementsSizeInBytes = m_Stride * m_ElementsCount;

  FBuffer stagingBuffer{};
  stagingBuffer.Allocate(m_ElementsSizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         m_Device, m_pPhysicalDeviceAttributes);
  stagingBuffer.Fill(pData, elementSizeof, elementsCount);
  
  FCommandBuffer commandBuffer = transferCommandPool.AllocatePrimaryCommandBuffer();
  commandBuffer.BeginOneTimeRecording();

  VkBufferCopy copyRegion{
    .srcOffset = 0, // Optional
    .dstOffset = 0, // Optional
    .size = m_ElementsSizeInBytes
  };
  vkCmdCopyBuffer(commandBuffer.GetHandle(), stagingBuffer.GetHandle(), m_Buffer, 1, &copyRegion);

  commandBuffer.EndRecording();

  transferQueue.Submit({}, {}, commandBuffer, {}, VK_NULL_HANDLE);
  transferQueue.WaitIdle();
}


b32 FBuffer::IsValid() const
{
  return m_Buffer != VK_NULL_HANDLE;
}


}
