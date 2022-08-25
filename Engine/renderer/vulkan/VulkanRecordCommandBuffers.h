
#ifndef UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
#define UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H


namespace uncanny
{


b32 recordClearColorImage(
    const std::vector<FImageVulkan>& renderTargetImages,
    const std::vector<VkCommandBuffer>& commandBuffers,
    u32 queueFamilyIndex);


b32 recordCopyRenderTargetIntoPresentableImage(const std::vector<FImageVulkan>& renderTargetImages,
                                               const std::vector<FImageVulkan>& presentableImages,
                                               const std::vector<VkCommandBuffer>& commandBuffers,
                                               u32 queueFamilyIndex);


}


#endif //UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
