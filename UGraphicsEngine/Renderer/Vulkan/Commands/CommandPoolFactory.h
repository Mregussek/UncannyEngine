
#ifndef UNCANNYENGINE_COMMANDPOOLFACTORY_H
#define UNCANNYENGINE_COMMANDPOOLFACTORY_H


#include <vector>
#include "CommandBuffer.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


class FCommandPoolFactory {

  friend class FCommandPool;

public:

  [[nodiscard]] std::vector<FCommandBuffer> AllocatePrimaryCommandBuffers(u32 count) const;

  [[nodiscard]] FCommandBuffer AllocateAndBeginSingleUseCommandBuffer() const;

private:

  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_COMMANDPOOLFACTORY_H
