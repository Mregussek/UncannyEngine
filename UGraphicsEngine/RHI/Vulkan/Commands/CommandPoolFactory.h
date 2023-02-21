
#ifndef UNCANNYENGINE_COMMANDPOOLFACTORY_H
#define UNCANNYENGINE_COMMANDPOOLFACTORY_H


#include <vector>
#include "CommandBuffer.h"


namespace uncanny::vulkan {


class FCommandPoolFactory {

  friend class FCommandPool;

public:

  [[nodiscard]] std::vector<FCommandBuffer> AllocateCommandBuffers() const;

  [[nodiscard]] FCommandBuffer AllocateSingleUseCommandBuffer() const;

private:

  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_COMMANDPOOLFACTORY_H
