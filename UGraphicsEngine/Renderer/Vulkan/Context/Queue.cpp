
#include "Queue.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"

namespace uncanny::vulkan
{


void FQueue::Initialize(VkQueue queue, FQueueFamilyIndex familyIndex)
{
  m_Queue = queue;
  m_FamilyIndex = familyIndex;
}


void FQueue::Submit(std::span<VkSemaphore> waitVkSemaphores, std::span<VkCommandBuffer> vkCommandBuffers,
                    std::span<VkSemaphore> signalVkSemaphores, VkPipelineStageFlags waitStageFlag,
                    VkFence vkFence) const
{
  VkSubmitInfo submitInfo{
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = static_cast<u32>(waitVkSemaphores.size()),
    .pWaitSemaphores = waitVkSemaphores.data(),
    .pWaitDstStageMask = &waitStageFlag,
    .commandBufferCount = static_cast<u32>(vkCommandBuffers.size()),
    .pCommandBuffers = vkCommandBuffers.data(),
    .signalSemaphoreCount = static_cast<u32>(signalVkSemaphores.size()),
    .pSignalSemaphores = signalVkSemaphores.data()
  };

  VkResult result = vkQueueSubmit(m_Queue, 1, &submitInfo, vkFence);
  AssertVkAndThrow(result);
}


}
