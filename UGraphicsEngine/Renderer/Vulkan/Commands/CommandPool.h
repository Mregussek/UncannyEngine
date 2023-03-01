
#ifndef UNCANNYENGINE_COMMANDPOOL_H
#define UNCANNYENGINE_COMMANDPOOL_H


#include <volk.h>
#include "CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Devices/LogicalDeviceAttributes.h"


namespace uncanny::vulkan {


class FCommandPool {
public:

  void Create(FQueueFamilyIndex queueFamilyIndex, VkDevice vkDevice);
  void Destroy();

  void Reset();

  [[nodiscard]] std::vector<FCommandBuffer> AllocatePrimaryCommandBuffers(u32 count) const;

  [[nodiscard]] FCommandBuffer AllocateAndBeginSingleUseCommandBuffer() const;

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };


};


}


#endif //UNCANNYENGINE_COMMANDPOOL_H