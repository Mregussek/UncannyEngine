
#ifndef UNCANNYENGINE_COMMANDBUFFER_H
#define UNCANNYENGINE_COMMANDBUFFER_H


#include <volk.h>


namespace uncanny::vulkan {


class FCommandBuffer {
public:

  FCommandBuffer() = delete;
  FCommandBuffer(VkDevice device, VkCommandBuffer commandBuffer);
  ~FCommandBuffer();

  void Free();

private:

  VkCommandBuffer m_CommandBuffer{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_COMMANDBUFFER_H
