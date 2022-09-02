
#ifndef UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
#define UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H


#include "RendererStructuresVulkan.h"
#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


b32 recordClearColorImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<VkCommandBuffer>& commandBuffers,
    u32 queueFamilyIndex);


b32 recordCopyRenderTargetIntoPresentableImage(const std::vector<FImageVulkan>& renderTargetImages,
                                               const std::vector<FImageVulkan>& presentableImages,
                                               const std::vector<VkCommandBuffer>& commandBuffers);


b32 recordClearScreenWithRenderPassForRenderTarget(
    const std::vector<FImageVulkan>& renderTargetImages, VkRenderPass renderPass,
    const std::vector<VkCommandBuffer>& commandBuffers);


b32 recordTriangleGraphicsPipelineForRenderTarget(
    const std::vector<FImageVulkan>& renderTargetImages, VkRenderPass renderPass,
    VkPipeline graphicsPipeline, VkViewport viewport, VkRect2D scissor,
    const std::vector<VkCommandBuffer>& commandBuffers);


b32 recordIndexedVertexBufferGraphicsPipelineForRenderTarget(
    const std::vector<FImageVulkan>& renderTargetImages, VkRenderPass renderPass,
    VkPipeline graphicsPipeline, VkPipelineLayout pipelineLayout,
    const std::vector<VkDescriptorSet>& descriptorSets, VkViewport viewport, VkRect2D scissor,
    FBufferVulkan* pVertexBuffer, FBufferVulkan* pIndexBuffer,
    const std::vector<VkCommandBuffer>& commandBuffers);


}


#endif //UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
