
#ifndef UNCANNYENGINE_COMMANDBUFFER_H
#define UNCANNYENGINE_COMMANDBUFFER_H


#include <volk.h>
#include <span>
#include "UTools/UTypes.h"
#undef MemoryBarrier


namespace uncanny::vulkan
{


class FRayTracingPipeline;


/// @brief FCommandBuffer is wrapper class for VkCommandBuffer. User is responsible for lifetime, but FCommandPool
/// creates object. Optionally destructor is called and freed.
/// @details FCommandPool is responsible for allocation command buffer. This class can only be freed.
/// All methods should be straightforward and I decided not to document them as user should be aware of
/// VkCommandBuffer functionality.
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

  void BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
                       std::span<VkClearValue> clearValues);
  void EndRenderPass();

  void MemoryBarrier(VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage,
                     VkPipelineStageFlags dstStage);
  void ImageMemoryBarrier(VkImage image, VkAccessFlags srcFlags, VkAccessFlags dstFlags, VkImageLayout oldLayout,
                          VkImageLayout newLayout, VkImageSubresourceRange subresourceRange,
                          VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
  void ClearColorImage(VkImage image, VkClearColorValue clearValue, VkImageSubresourceRange subresourceRange);

  void CopyImage(VkImage srcImage, VkImage dstImage, VkImageSubresourceLayers subresourceLayers, VkExtent2D extent);
  void CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageSubresourceLayers subresourceLayers,
                         VkExtent2D extent2D);

  void BuildAccelerationStructure(const VkAccelerationStructureBuildGeometryInfoKHR* pBuildGeometryInfo,
                                  const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos);

  void BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline);

  void BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet);
  void BindDescriptorSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout,
                          std::span<VkDescriptorSet> descriptorSets);

  void BindVertexBuffers(std::span<VkBuffer> vertexBuffers);
  void BindIndexBuffer(VkBuffer indexBuffer, VkIndexType indexType);

  void SetViewport(VkViewport viewport);
  void SetViewports(std::span<VkViewport> viewports);

  void SetScissor(VkRect2D scissor);

  void PushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags shaderStage, u32 size, const void* pConstants);

  void TraceRays(const FRayTracingPipeline* pRayTracingPipeline, VkExtent3D extent3D);

  void DrawIndexed(u32 elementsCount, u32 firstIndex, u32 indexOffset, i32 vertexOffset, u32 firstInstance);

  [[nodiscard]] VkCommandBuffer GetHandle() const { return m_CommandBuffer; }
  [[nodiscard]] VkPipelineStageFlags GetLastWaitPipelineStage() const { return m_LastWaitStageFlag; }

private:

  VkCommandBuffer m_CommandBuffer{ VK_NULL_HANDLE };
  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkPipelineStageFlags m_LastWaitStageFlag{ VK_PIPELINE_STAGE_NONE };

};


}


#endif //UNCANNYENGINE_COMMANDBUFFER_H
