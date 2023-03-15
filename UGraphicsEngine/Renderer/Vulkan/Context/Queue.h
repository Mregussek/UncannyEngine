
#ifndef UNCANNYENGINE_QUEUE_H
#define UNCANNYENGINE_QUEUE_H


#include <volk.h>
#include "Typedefs.h"
#include <span>


namespace uncanny::vulkan
{


class FQueue
{
public:

  void Initialize(VkQueue queue, FQueueFamilyIndex familyIndex);

  void Submit(std::span<VkSemaphore> waitVkSemaphores, std::span<VkCommandBuffer> vkCommandBuffers,
              std::span<VkSemaphore> signalVkSemaphores, VkPipelineStageFlags waitStageFlag, VkFence vkFence) const;

  [[nodiscard]] VkQueue GetHandle() const { return m_Queue; }
  [[nodiscard]] FQueueFamilyIndex GetFamilyIndex() const { return m_FamilyIndex; }

private:

  VkQueue m_Queue{ VK_NULL_HANDLE };
  FQueueFamilyIndex m_FamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_QUEUE_H
