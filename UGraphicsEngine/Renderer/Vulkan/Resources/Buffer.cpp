
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


void FBuffer::Allocate(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags)
{
  VkBufferCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .size = size,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr
  };

  VkResult result = vkCreateBuffer(m_Device, &createInfo, nullptr, &m_Buffer);
  AssertVkAndThrow(result);

  VkMemoryRequirements memoryRequirements{};
  vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memoryRequirements);

  m_Memory.Allocate(m_Device, m_pPhysicalDeviceAttributes->GetMemoryProperties(), memoryRequirements,
                    memoryFlags);

  VkDeviceSize memoryOffset{ 0 };
  vkBindBufferMemory(m_Device, m_Buffer, m_Memory.GetHandle(), memoryOffset);
}


void FBuffer::Free()
{
  if (m_Buffer != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_Device, m_Buffer, nullptr);
  }

  m_Memory.Free();
}


}
