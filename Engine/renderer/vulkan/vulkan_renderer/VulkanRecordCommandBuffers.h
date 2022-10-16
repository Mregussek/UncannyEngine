
#ifndef UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
#define UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny::vkf
{


class FImageVulkan;


}
namespace uncanny
{


b32 recordCopyRenderTargetIntoPresentableImage(
    const std::vector<vkf::FImageVulkan>& renderTargetImages,
    const std::vector<vkf::FImageVulkan>& presentableImages,
    const std::vector<VkCommandBuffer>& commandBuffers);


}


#endif //UNCANNYENGINE_VULKANRECORDCOMMANDBUFFERS_H
