
#ifndef UNCANNYENGINE_QUEUE_H
#define UNCANNYENGINE_QUEUE_H


#include <volk.h>
#include "Typedefs.h"
#include <span>


namespace uncanny::vulkan
{


class FCommandBuffer;


class FQueue
{

  friend class FLogicalDevice;

public:

  void Submit(std::span<VkSemaphore> waitVkSemaphores, const FCommandBuffer& commandBuffer,
              std::span<VkSemaphore> signalVkSemaphores, VkFence vkFence) const;

  void WaitIdle() const;

  [[nodiscard]] VkQueue GetHandle() const { return m_Queue; }
  [[nodiscard]] FQueueFamilyIndex GetFamilyIndex() const { return m_FamilyIndex; }

private:

  void Initialize(VkQueue queue, FQueueFamilyIndex familyIndex);


  VkQueue m_Queue{ VK_NULL_HANDLE };
  FQueueFamilyIndex m_FamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_QUEUE_H
