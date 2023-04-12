
#ifndef UNCANNYENGINE_COMMANDBUFFER_H
#define UNCANNYENGINE_COMMANDBUFFER_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FRayTracingPipeline;


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

  void BuildAccelerationStructure(const VkAccelerationStructureBuildGeometryInfoKHR* pBuildGeometryInfo,
                                  const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos);

  void BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline);

  void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet);

  void TraceRays(const FRayTracingPipeline* pRayTracingPipeline, VkExtent3D extent3D);

  [[nodiscard]] VkCommandBuffer GetHandle() const { return m_CommandBuffer; }
  [[nodiscard]] VkPipelineStageFlags GetLastWaitPipelineStage() const { return m_LastWaitStageFlag; }

private:

  VkCommandBuffer m_CommandBuffer{ VK_NULL_HANDLE };
  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkPipelineStageFlags m_LastWaitStageFlag{ VK_PIPELINE_STAGE_NONE };
  b8 m_Recording{ UFALSE };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_COMMANDBUFFER_H
