
#ifndef UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
#define UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


class FBufferVulkan;


struct FShaderModuleVertexDataVulkan {
  VkShaderModule handle{ VK_NULL_HANDLE };
  VkVertexInputBindingDescription inputBindingDescription{};
  std::vector<VkVertexInputAttributeDescription> inputAttrDescVector{};
  std::vector<VkDescriptorSetLayoutBinding> layoutBindingVector{};
  const char* path{ "" };
};


struct FShaderModuleFragmentDataVulkan {
  VkShaderModule handle{ VK_NULL_HANDLE };
  const char* path{ "" };
};


struct FShaderModuleDataVulkan {
  std::vector<VkPipelineShaderStageCreateInfo> shaderStagesVector{};
  const char* logInfo{ "" };
};


struct FGraphicsPipelineShaderModulesCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  const char* vertexPath{ "" };
  const char* fragmentPath{ "" };
  const char* logInfo{ "" };
};


class FGraphicsPipelineShaderModulesVulkan {
public:

  b32 create(const FGraphicsPipelineShaderModulesCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  [[nodiscard]] const FShaderModuleDataVulkan& getData() const { return mData; }
  [[nodiscard]] const FShaderModuleVertexDataVulkan& getVertexData() const { return mVertexData; }
  [[nodiscard]] const FShaderModuleFragmentDataVulkan& getFragmentData() const {
    return mFragmentData;
  }

private:

  FShaderModuleDataVulkan mData{};
  FShaderModuleVertexDataVulkan mVertexData{};
  FShaderModuleFragmentDataVulkan mFragmentData{};

};


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


struct FGraphicsPipelineDescriptorsDataVulkan {
  VkDescriptorPool pool{ VK_NULL_HANDLE };
  std::vector<VkDescriptorSet> descriptorSetVector{};
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


struct FGraphicsPipelineDataVulkan {
  VkPipeline pipeline{ VK_NULL_HANDLE };
  const char* logInfo{ "" };
};


struct FGraphicsPipelineCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  VkRenderPass renderPass{ VK_NULL_HANDLE };
  FGraphicsPipelineShaderModulesCreateDependenciesVulkan* pShaderDeps{ nullptr };
  const char* logInfo{ "" };
};


class FGraphicsPipelineVulkan {
public:

  b32 create(const FGraphicsPipelineCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  void passCameraUboToDescriptor(VkDevice device, FBufferVulkan* pCameraUBO);

  [[nodiscard]] VkPipeline getPipelineHandle() const { return mData.pipeline; }
  [[nodiscard]] VkPipelineLayout getPipelineLayout() const {
    return mLayout.getData().pipelineLayout;
  }
  [[nodiscard]] const std::vector<VkDescriptorSet>& getDescriptorSets() const {
    return mDescriptors.getData().descriptorSetVector;
  }

private:

  FGraphicsPipelineDataVulkan mData{};
  FGraphicsPipelineShaderModulesVulkan mShaders{};
  FGraphicsPipelineLayoutVulkan mLayout{};
  FGraphicsPipelineDescriptorsVulkan mDescriptors{};

};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
