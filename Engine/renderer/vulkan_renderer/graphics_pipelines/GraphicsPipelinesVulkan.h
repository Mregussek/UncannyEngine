
#ifndef UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
#define UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>
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
  VkRenderPass renderPass{ VK_NULL_HANDLE };
  FShaderModulesCreateDependenciesVulkan* pShaderDeps{ nullptr };
  const char* logInfo{ "" };
};


struct FGraphicsPipelineRecordCommandsDependencies {
  const std::vector<FImageVulkan>* pRenderTargets{ nullptr };
  VkRenderPass renderPass{ VK_NULL_HANDLE };
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

  [[nodiscard]] VkPipeline getPipelineHandle() const { return mData.pipeline; }
  [[nodiscard]] VkPipelineLayout getPipelineLayout() const {
    return mLayout.getData().pipelineLayout;
  }
  [[nodiscard]] std::vector<VkDescriptorSet> getDescriptorSets() const {
    return { mDescriptors.getData().cameraDescriptorSet };
  }

private:

  FGraphicsPipelineDataVulkan mData{};
  FShaderModulesVulkan mShaders{};
  FGraphicsPipelineLayoutVulkan mLayout{};
  FGraphicsPipelineDescriptorsVulkan mDescriptors{};

};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
