
#ifndef UNCANNYENGINE_GRAPHICSPIPELINELAYOUTVULKAN_H
#define UNCANNYENGINE_GRAPHICSPIPELINELAYOUTVULKAN_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny
{


struct FShaderModuleVertexDataVulkan;
struct FShaderModuleFragmentDataVulkan;


struct FGraphicsPipelineLayoutDataVulkan {
  VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
  VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
  const char* logInfo{ "" };
};


struct FGraphicsPipelineLayoutCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  const FShaderModuleVertexDataVulkan* pVertexData{ nullptr };
  const FShaderModuleFragmentDataVulkan* pFragmentData{ nullptr };
  const char* logInfo{ "" };
};


class FGraphicsPipelineLayoutVulkan {
public:

  b32 create(const FGraphicsPipelineLayoutCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  [[nodiscard]] const FGraphicsPipelineLayoutDataVulkan& getData() const { return mData; }

private:

  FGraphicsPipelineLayoutDataVulkan mData{};

};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINELAYOUTVULKAN_H
