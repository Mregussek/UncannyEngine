
#include "CommandPoolFactory.h"


namespace uncanny::vulkan {


std::vector<FCommandBuffer> FCommandPoolFactory::AllocateCommandBuffers() const {
  return {};
}


FCommandBuffer FCommandPoolFactory::AllocateSingleUseCommandBuffer() const {
  return {};
}


}
