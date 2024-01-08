
#ifndef UNCANNYENGINE_COMMANDPOOL_H
#define UNCANNYENGINE_COMMANDPOOL_H


#include <volk.h>
#include <vector>
#include "CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Typedefs.h"


namespace uncanny::vulkan
{


/// @brief FCommandPool is a wrapper class for VkCommandPool functionality.
/// @details User is completely responsible for lifetime via Create() and Destroy() methods.
/// Destructor calls Destroy() method to ensure that object will be deleted.
class FCommandPool
{
public:

  FCommandPool() = default;

  FCommandPool(const FCommandPool& other) = delete;
  FCommandPool(FCommandPool&& other) = delete;

  FCommandPool& operator=(const FCommandPool& other) = delete;
  FCommandPool& operator=(FCommandPool& other) = delete;

  ~FCommandPool();

public:

  void Create(FQueueFamilyIndex queueFamilyIndex, VkDevice vkDevice, VkCommandPoolCreateFlags flags);
  void Destroy();

  void Reset();

  [[nodiscard]] FCommandBuffer AllocatePrimaryCommandBuffer() const;
  [[nodiscard]] std::vector<FCommandBuffer> AllocatePrimaryCommandBuffers(u32 count) const;

// Getters
public:

  [[nodiscard]] FQueueFamilyIndex GetFamilyIndex() const { return m_QueueFamilyIndex; }

// Members
private:

  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  FQueueFamilyIndex m_QueueFamilyIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_COMMANDPOOL_H
