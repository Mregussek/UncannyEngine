
#ifndef UNCANNYENGINE_SHADERMODULES_H
#define UNCANNYENGINE_SHADERMODULES_H


#include <volk.h>
#include <utilities/Variables.h>
#include <vector>
#include <math/mat4.h>


namespace uncanny::vkf
{


class FBufferVulkan;


}
namespace uncanny
{


class FCamera;
class FMesh;


struct FShaderModuleUniformVulkan {
  mat4 matrixMVP{};
  mat4 matrixModel{};
  mat4 matrixView{};
  mat4 matrixProjection{};
  mat4 matrixMeshLocalTransform{};
  mat4 matrixMeshWorldTransform{};
};


struct FShaderModuleVertexDataVulkan {
  VkShaderModule handle{ VK_NULL_HANDLE };
  VkVertexInputBindingDescription inputBindingDescription{};
  std::vector<VkVertexInputAttributeDescription> inputAttrDescVector{};
  VkDescriptorSetLayoutBinding descriptorLayoutBinding{};
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
  VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
  const char* logInfo{ "" };
};


struct FShaderModulesCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  const char* vertexPath{ "" };
  const char* fragmentPath{ "" };
  const char* logInfo{ "" };
};


struct FShaderWriteIntoDescriptorSetDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  vkf::FBufferVulkan* pUniformBuffer{ nullptr };
};


class FShaderModulesVulkan {
public:

  b32 create(const FShaderModulesCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  static void fillShaderUniform(FCamera* pCamera, FMesh* pMesh,
                                FShaderModuleUniformVulkan* pOutShaderUniform);

  void writeDataIntoDescriptorSet(const FShaderWriteIntoDescriptorSetDependenciesVulkan& deps);

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
