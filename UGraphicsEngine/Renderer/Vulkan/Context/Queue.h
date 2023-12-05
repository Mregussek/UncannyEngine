
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

  // FLogicalDevice is the only class that can initialize FQueue
  friend class FLogicalDevice;

public:

  FQueue() = default;

  FQueue(const FQueue& other) = delete;
  FQueue(FQueue&& other) = delete;

  FQueue& operator=(const FQueue& other) = delete;
  FQueue& operator=(FQueue&& other) = delete;

public:

  /// @brief Submits a command buffer to the queue
  void Submit(std::span<VkSemaphore> waitVkSemaphores, std::span<VkPipelineStageFlags> waitStageFlags,
              const FCommandBuffer& commandBuffer, std::span<VkSemaphore> signalVkSemaphores, VkFence vkFence) const;

  /// @brief Waits for the queue to finish its work
  void WaitIdle() const;

// Getters
public:

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
