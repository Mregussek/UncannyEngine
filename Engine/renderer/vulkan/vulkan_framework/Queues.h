
#ifndef UNCANNYENGINE_QUEUES_H
#define UNCANNYENGINE_QUEUES_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf {


struct FQueuesInitDependenciesVulkan {
  VkDevice logicalDevice{ VK_NULL_HANDLE };
  u32 queueFamilyIndexGraphics{ UUNUSED };
  u32 availableQueuesGraphics{ UUNUSED };
  u32 queueFamilyIndexTransfer{ UUNUSED };
  u32 availableQueuesTransfer{ UUNUSED };
};


class FQueuesVulkan {
public:

  b32 init(const FQueuesInitDependenciesVulkan& deps);
  void terminate();

  [[nodiscard]] VkQueue QueueRendering() const { return m_VkQueueRendering; }
  [[nodiscard]] VkQueue QueuePresentation() const { return m_VkQueuePresentation; }
  [[nodiscard]] VkQueue QueueTransfer() const { return m_VkQueueTransfer; }

private:

  VkQueue m_VkQueueRendering{ VK_NULL_HANDLE };
  VkQueue m_VkQueuePresentation{ VK_NULL_HANDLE };
  VkQueue m_VkQueueTransfer{ VK_NULL_HANDLE };
  u32 m_QueueIndexRendering{ UUNUSED };
  u32 m_QueueIndexPresentation{ UUNUSED };
  u32 m_QueueIndexTransfer{ UUNUSED };

};


}


#endif //UNCANNYENGINE_QUEUES_H
