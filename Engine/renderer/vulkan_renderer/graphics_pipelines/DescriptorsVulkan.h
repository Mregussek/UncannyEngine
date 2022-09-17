
#ifndef UNCANNYENGINE_GRAPHICSPIPELINEDESCRIPTORSVULKAN_H
#define UNCANNYENGINE_GRAPHICSPIPELINEDESCRIPTORSVULKAN_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


struct FShaderModuleVertexDataVulkan;
struct FGraphicsPipelineLayoutDataVulkan;


struct FGraphicsPipelineDescriptorsDataVulkan {
  VkDescriptorPool pool{ VK_NULL_HANDLE };
  VkDescriptorSet cameraDescriptorSet{ VK_NULL_HANDLE };
  const char* logInfo{ "" };
};


struct FGraphicsPipelineDescriptorsCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  const FShaderModuleVertexDataVulkan* pVertexShaderData{ nullptr };
  const FGraphicsPipelineLayoutDataVulkan* pLayoutData{ nullptr };
  const char* logInfo{ "" };
};


class FGraphicsPipelineDescriptorsVulkan {
public:

  b32 create(const FGraphicsPipelineDescriptorsCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  [[nodiscard]] const FGraphicsPipelineDescriptorsDataVulkan& getData() const { return mData; }

private:

  FGraphicsPipelineDescriptorsDataVulkan mData{};

};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINEDESCRIPTORSVULKAN_H
