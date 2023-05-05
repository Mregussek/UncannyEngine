
#ifndef UNCANNYENGINE_COMMANDPOOL_H
#define UNCANNYENGINE_COMMANDPOOL_H


#include <volk.h>
#include <vector>
#include "CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Typedefs.h"


namespace uncanny::vulkan
{


/// @brief FCommandPool is a wrapper class for VkCommandPool functionality. User is completely responsible for lifetime
/// via Create() and Destroy() methods.
class FCommandPool
{
public:

  ~FCommandPool();

  void Create(FQueueFamilyIndex queueFamilyIndex, VkDevice vkDevice, VkCommandPoolCreateFlags flags);
  void Destroy();

  void Reset();

  [[nodiscard]] FCommandBuffer AllocatePrimaryCommandBuffer() const;
  [[nodiscard]] std::vector<FCommandBuffer> AllocatePrimaryCommandBuffers(u32 count) const;

  [[nodiscard]] FQueueFamilyIndex GetFamilyIndex() const { return m_QueueFamilyIndex; }

private:

  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  FQueueFamilyIndex m_QueueFamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_COMMANDPOOL_H
