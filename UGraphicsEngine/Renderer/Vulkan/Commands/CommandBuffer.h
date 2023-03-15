
#ifndef UNCANNYENGINE_COMMANDBUFFER_H
#define UNCANNYENGINE_COMMANDBUFFER_H


#include <volk.h>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FCommandBuffer
{
public:

  FCommandBuffer() = default;
  FCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBuffer vkCommandBuffer);
  ~FCommandBuffer();

  void Free();

  void BeginRecording();
  void BeginOneTimeRecording();
  void EndRecording();

  void ImageMemoryBarrier(VkImage image, VkAccessFlags srcFlags, VkAccessFlags dstFlags, VkImageLayout oldLayout,
                          VkImageLayout newLayout, VkImageSubresourceRange subresourceRange,
                          VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
  void ClearColorImage(VkImage image, VkClearColorValue clearValue, VkImageSubresourceRange subresourceRange);

  [[nodiscard]] VkCommandBuffer GetHandle() const { return m_CommandBuffer; }

private:

  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkCommandBuffer m_CommandBuffer{ VK_NULL_HANDLE };
  b8 m_Recording{ UFALSE };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_COMMANDBUFFER_H
