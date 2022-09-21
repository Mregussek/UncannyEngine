
#ifndef UNCANNYENGINE_SHADERMODULES_H
#define UNCANNYENGINE_SHADERMODULES_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


struct FShaderModuleVertexDataVulkan {
  VkShaderModule handle{ VK_NULL_HANDLE };
  VkVertexInputBindingDescription inputBindingDescription{};
  std::vector<VkVertexInputAttributeDescription> inputAttrDescVector{};
  VkDescriptorSetLayoutBinding cameraDescriptorLayoutBinding{};
  const char* path{ "" };
};


struct FShaderModuleFragmentDataVulkan {
  VkShaderModule handle{ VK_NULL_HANDLE };
  const char* path{ "" };
};


struct FShaderModuleDataVulkan {
  std::vector<VkPipelineShaderStageCreateInfo> shaderStagesVector{};
  VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
  VkDescriptorPool pool{ VK_NULL_HANDLE };
  VkDescriptorSet cameraDescriptorSet{ VK_NULL_HANDLE };
  const char* logInfo{ "" };
};


struct FShaderModulesCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  const char* vertexPath{ "" };
  const char* fragmentPath{ "" };
  const char* logInfo{ "" };
};


class FShaderModulesVulkan {
public:

  b32 create(const FShaderModulesCreateDependenciesVulkan& deps);
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


}


#endif //UNCANNYENGINE_SHADERMODULES_H
