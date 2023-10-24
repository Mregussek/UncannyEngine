
#ifndef UNCANNYENGINE_QUEUE_H
#define UNCANNYENGINE_QUEUE_H


#include <volk.h>
#include "Typedefs.h"
#include <span>


namespace uncanny::vulkan
{


class FCommandBuffer;


/// @brief FQueue is a wrapper class for VkQueue. It is initialized only by FLogicalDevice, will be invalid otherwise.
class FQueue
{

  friend class FLogicalDevice;

public:

  void Submit(std::span<VkSemaphore> waitVkSemaphores, std::span<VkPipelineStageFlags> waitStageFlags,
              const FCommandBuffer& commandBuffer, std::span<VkSemaphore> signalVkSemaphores, VkFence vkFence) const;

  void WaitIdle() const;

  [[nodiscard]] VkQueue GetHandle() const { return m_Queue; }
  [[nodiscard]] FQueueFamilyIndex GetFamilyIndex() const { return m_FamilyIndex; }

private:

  void Initialize(VkQueue queue, FQueueFamilyIndex familyIndex);

private:

  VkQueue m_Queue{ VK_NULL_HANDLE };
  FQueueFamilyIndex m_FamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_QUEUE_H
