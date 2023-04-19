
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


void FBuffer::Allocate(VkDeviceSize memorySize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags)
{
  m_MemoryFlags = memoryFlags;
  m_MemorySize = memorySize;

  VkBufferCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .size = m_MemorySize,
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

  m_Memory.Allocate(m_Device, m_pPhysicalDeviceAttributes->GetMemoryProperties(), memoryRequirements, m_MemoryFlags,
                    useDeviceAddress);

  constexpr VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(m_Device, m_Buffer, m_Memory.GetHandle(), memoryOffset);

  if (useDeviceAddress)
  {
    VkBufferDeviceAddressInfoKHR addressInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext = nullptr,
        .buffer = m_Buffer
    };
    m_Address = vkGetBufferDeviceAddressKHR(m_Device, &addressInfo);
  }
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


void FBuffer::Fill(void* pData, u32 elementSizeof, u32 elementsCount)
{
  m_Stride = elementSizeof;
  m_ElementsCount = elementsCount;
  m_ElementsSizeInBytes = m_Stride * m_ElementsCount;

  VkDeviceSize offset{ 0 };
  VkMemoryMapFlags flags{ 0 };
  void* pMapPtr{ nullptr };

  vkMapMemory(m_Device, m_Memory.GetHandle(), offset, m_MemorySize, flags, &pMapPtr);
  memcpy(pMapPtr, pData, m_ElementsSizeInBytes);
  vkUnmapMemory(m_Device, m_Memory.GetHandle());
}


void FBuffer::FillStaged(void* pData, u32 elementSizeof, u32 elementsCount, const FCommandPool& transferCommandPool,
                         const FQueue& transferQueue)
{
  VkDeviceSize dataSizeof = elementSizeof * elementsCount;
  FBuffer stagingBuffer{ m_pPhysicalDeviceAttributes, m_Device };
  stagingBuffer.Allocate(dataSizeof, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  stagingBuffer.Fill(pData, elementSizeof, elementsCount);

  FCommandBuffer commandBuffer = transferCommandPool.AllocatePrimaryCommandBuffer();
  commandBuffer.BeginOneTimeRecording();

  VkBufferCopy copyRegion{
    .srcOffset = 0, // Optional
    .dstOffset = 0, // Optional
    .size = dataSizeof
  };

  vkCmdCopyBuffer(commandBuffer.GetHandle(), stagingBuffer.m_Buffer, m_Buffer, 1, &copyRegion);

  commandBuffer.EndRecording();

  transferQueue.Submit({}, {}, commandBuffer, {}, VK_NULL_HANDLE);
  transferQueue.WaitIdle();

  commandBuffer.Free();
  stagingBuffer.Free();
}


}
