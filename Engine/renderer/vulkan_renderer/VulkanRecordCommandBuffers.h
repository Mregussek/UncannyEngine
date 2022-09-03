
#ifndef UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
#define UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H


#include "RendererStructuresVulkan.h"
#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


b32 recordIndexedVertexBufferGraphicsPipelineForRenderTarget(
    const std::vector<FImageVulkan>& renderTargetImages, VkRenderPass renderPass,
    const FGraphicsPipelineVulkan& graphicsPipeline, const FBufferVulkan& vertexBuffer,
    const FBufferVulkan& indexBuffer, const std::vector<VkCommandBuffer>& commandBuffers);


b32 recordCopyRenderTargetIntoPresentableImage(const std::vector<FImageVulkan>& renderTargetImages,
                                               const std::vector<FImageVulkan>& presentableImages,
                                               const std::vector<VkCommandBuffer>& commandBuffers);


}


#endif //UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
