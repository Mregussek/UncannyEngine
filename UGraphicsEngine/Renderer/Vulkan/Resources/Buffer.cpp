
#include "Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/PhysicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FBuffer::FBuffer(const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, VkDevice vkDevice)
  : m_pPhysicalDeviceAttributes(pPhysicalDeviceAttributes),
  m_Device(vkDevice)
{
}


FBuffer::~FBuffer()
{
  Free();
}


void FBuffer::Allocate(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags)
{
  m_MemoryFlags = memoryFlags;
  m_Size = size;

  VkBufferCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .size = m_Size,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr
  };

  VkResult result = vkCreateBuffer(m_Device, &createInfo, nullptr, &m_Buffer);
  AssertVkAndThrow(result);

  VkMemoryRequirements memoryRequirements{};
  vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memoryRequirements);

  m_Memory.Allocate(m_Device, m_pPhysicalDeviceAttributes->GetMemoryProperties(), memoryRequirements, m_MemoryFlags);

  VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(m_Device, m_Buffer, m_Memory.GetHandle(), memoryOffset);
}


void FBuffer::Free()
{
  if (m_Freed)
  {
    return;
  }
  if (m_Buffer != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_Device, m_Buffer, nullptr);
  }
  m_Memory.Free();
  m_Freed = UTRUE;
}


void FBuffer::Fill(void* pData, VkDeviceSize dataSize)
{
  VkDeviceSize offset{ 0 };
  VkMemoryMapFlags flags{ 0 };
  void* pMapPtr{ nullptr };
  vkMapMemory(m_Device, m_Memory.GetHandle(), offset, m_Size, flags, &pMapPtr);
  memcpy(pMapPtr, pData, dataSize);
  vkUnmapMemory(m_Device, m_Memory.GetHandle());
}


void FBuffer::FillStaged(void* pData, VkDeviceSize dataSize, const FCommandPool& transferCommandPool,
                         const FQueue& transferQueue)
{
  FBuffer stagingBuffer{ m_pPhysicalDeviceAttributes, m_Device };
  stagingBuffer.Allocate(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  stagingBuffer.Fill(pData, dataSize);

  FCommandBuffer commandBuffer = transferCommandPool.AllocateCommandBuffer();
  commandBuffer.BeginOneTimeRecording();

  VkBufferCopy copyRegion{
    .srcOffset = 0, // Optional
    .dstOffset = 0, // Optional
    .size = dataSize
  };

  vkCmdCopyBuffer(commandBuffer.GetHandle(), stagingBuffer.m_Buffer, m_Buffer, 1, &copyRegion);

  commandBuffer.EndRecording();

  transferQueue.Submit({}, commandBuffer, {}, VK_NULL_HANDLE);
  transferQueue.WaitIdle();

  commandBuffer.Free();
  stagingBuffer.Free();
}


}
