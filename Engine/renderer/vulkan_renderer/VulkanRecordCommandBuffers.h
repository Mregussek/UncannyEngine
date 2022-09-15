
#ifndef UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
#define UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{

class FImageVulkan;
class FGraphicsPipelineVulkan;
class FBufferVulkan;


struct FRecordCommandsForIndexVertexBuffersDependencies {
  const std::vector<FImageVulkan>* pRenderTargets{ nullptr };
  VkRenderPass renderPass{ VK_NULL_HANDLE };
  const FGraphicsPipelineVulkan* pGraphicsPipeline{ nullptr };
  const FBufferVulkan* pVertexBuffer{ nullptr };
  const FBufferVulkan* pIndexBuffer{ nullptr };
  const std::vector<VkCommandBuffer>* pCommandBuffers{ nullptr };
  VkViewport viewport{};
  VkRect2D scissor{};
};


b32 recordIndexedVertexBufferGraphicsPipelineForRenderTarget(
    const FRecordCommandsForIndexVertexBuffersDependencies& deps);


b32 recordCopyRenderTargetIntoPresentableImage(const std::vector<FImageVulkan>& renderTargetImages,
                                               const std::vector<FImageVulkan>& presentableImages,
                                               const std::vector<VkCommandBuffer>& commandBuffers);


}


#endif //UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
