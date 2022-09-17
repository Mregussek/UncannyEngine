
#ifndef UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
#define UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>
#include "RenderPassVulkan.h"
#include "ShaderModulesVulkan.h"
#include "LayoutVulkan.h"
#include "DescriptorsVulkan.h"


namespace uncanny
{


class FImageVulkan;
class FBufferVulkan;


struct FGraphicsPipelineDataVulkan {
  VkPipeline pipeline{ VK_NULL_HANDLE };
  const char* logInfo{ "" };
};


struct FGraphicsPipelineCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  FRenderPassCreateDependenciesVulkan* pRenderPassDeps{ nullptr };
  FShaderModulesCreateDependenciesVulkan* pShaderDeps{ nullptr };
  const char* logInfo{ "" };
};


struct FGraphicsPipelineRecordCommandsDependencies {
  const std::vector<FImageVulkan>* pRenderTargets{ nullptr };
  const FBufferVulkan* pVertexBuffer{ nullptr };
  const FBufferVulkan* pIndexBuffer{ nullptr };
  const std::vector<VkCommandBuffer>* pCommandBuffers{ nullptr };
};


class FGraphicsPipelineVulkan {
public:

  b32 create(const FGraphicsPipelineCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  void passCameraUboToDescriptor(VkDevice device, FBufferVulkan* pCameraUBO);

  b32 recordUsageCommands(const FGraphicsPipelineRecordCommandsDependencies& deps);

  [[nodiscard]] FRenderPassDataVulkan getRenderPassData() const { return mRenderPass.getData(); }

private:

  FGraphicsPipelineDataVulkan mData{};
  FRenderPassVulkan mRenderPass{};
  FShaderModulesVulkan mShaders{};
  FGraphicsPipelineLayoutVulkan mLayout{};
  FGraphicsPipelineDescriptorsVulkan mDescriptors{};

};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
