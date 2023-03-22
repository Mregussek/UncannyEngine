
#include "Queue.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"

namespace uncanny::vulkan
{


void FQueue::Initialize(VkQueue queue, FQueueFamilyIndex familyIndex)
{
  m_Queue = queue;
  m_FamilyIndex = familyIndex;
}


void FQueue::Submit(std::span<VkSemaphore> waitVkSemaphores, std::span<VkPipelineStageFlags> waitStageFlags,
                    const FCommandBuffer& commandBuffer, std::span<VkSemaphore> signalVkSemaphores,
                    VkFence vkFence) const
{
  VkCommandBuffer vkCmdBuf = commandBuffer.GetHandle();

  VkSubmitInfo submitInfo{
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = static_cast<u32>(waitVkSemaphores.size()),
    .pWaitSemaphores = waitVkSemaphores.data(),
    .pWaitDstStageMask = waitStageFlags.data(),
    .commandBufferCount = 1,
    .pCommandBuffers = &vkCmdBuf,
    .signalSemaphoreCount = static_cast<u32>(signalVkSemaphores.size()),
    .pSignalSemaphores = signalVkSemaphores.data()
  };

  VkResult result = vkQueueSubmit(m_Queue, 1, &submitInfo, vkFence);
  AssertVkAndThrow(result);
}


void FQueue::WaitIdle() const
{
  vkQueueWaitIdle(m_Queue);
}


}
