
#ifndef UNCANNYENGINE_COMMANDBUFFER_H
#define UNCANNYENGINE_COMMANDBUFFER_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FCommandBuffer
{
public:

  FCommandBuffer() = default;
  FCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBuffer vkCommandBuffer);

  void Free();

  void BeginRecording();
  void BeginOneTimeRecording();
  void EndRecording();

  void ImageMemoryBarrier(VkImage image, VkAccessFlags srcFlags, VkAccessFlags dstFlags, VkImageLayout oldLayout,
                          VkImageLayout newLayout, VkImageSubresourceRange subresourceRange,
                          VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
  void ClearColorImage(VkImage image, VkClearColorValue clearValue, VkImageSubresourceRange subresourceRange);

  void CopyImage(VkImage srcImage, VkImage dstImage, VkImageSubresourceLayers subresourceLayers, VkExtent2D extent);

  [[nodiscard]] VkCommandBuffer GetHandle() const { return m_CommandBuffer; }
  [[nodiscard]] const std::vector<VkPipelineStageFlags>& GetWaitPipelineStages() const
  {
    return m_WaitPipelineStages;
  }

private:

  void AddPipelineStage(VkPipelineStageFlags stageFlags);


  VkCommandBuffer m_CommandBuffer{ VK_NULL_HANDLE };
  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  std::vector<VkPipelineStageFlags> m_WaitPipelineStages{};
  b8 m_Recording{ UFALSE };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_COMMANDBUFFER_H
