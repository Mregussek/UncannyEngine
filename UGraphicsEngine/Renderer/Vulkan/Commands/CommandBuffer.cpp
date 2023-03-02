
#include "CommandBuffer.h"


namespace uncanny::vulkan
{


FCommandBuffer::FCommandBuffer(VkDevice device, VkCommandBuffer commandBuffer)
  : m_CommandBuffer(commandBuffer),
  m_Device(device)
{

}


FCommandBuffer::~FCommandBuffer()
{

}


void FCommandBuffer::Free()
{

}


}
