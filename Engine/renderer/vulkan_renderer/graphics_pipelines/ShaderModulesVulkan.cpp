
#include "ShaderModulesVulkan.h"
#include <filesystem/FileManager.h>
#include <renderer/Mesh.h>
#include <utilities/Logger.h>


namespace uncanny
{


static b32 createShaderModule(const char* path, VkDevice device, VkShaderModule* pOutShaderModule);
static b32 closeShaderModule(VkDevice device, VkShaderModule* pShaderModule, const char* logInfo);


b32 FShaderModulesVulkan::create(const FShaderModulesCreateDependenciesVulkan& deps) {
  UTRACE("Creating shader modules for graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

  std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindingVector(1);
  descriptorLayoutBindingVector[0].binding = 0;
  descriptorLayoutBindingVector[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorLayoutBindingVector[0].descriptorCount = 1;
  descriptorLayoutBindingVector[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  descriptorLayoutBindingVector[0].pImmutableSamplers = nullptr;

  VkVertexInputBindingDescription vertexInputBindingDescription{};
  vertexInputBindingDescription.binding = 0;
  vertexInputBindingDescription.stride = sizeof(FVertex);
  vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> vertexInputAttrDescVector(2);
  {
    vertexInputAttrDescVector[0].location = 0;
    vertexInputAttrDescVector[0].binding = 0;
    vertexInputAttrDescVector[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttrDescVector[0].offset = offsetof(FVertex, position);
  }
  {
    vertexInputAttrDescVector[1].location = 1;
    vertexInputAttrDescVector[1].binding = 0;
    vertexInputAttrDescVector[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttrDescVector[1].offset = offsetof(FVertex, color);
  }

  mVertexData.inputBindingDescription = vertexInputBindingDescription;
  mVertexData.inputAttrDescVector = vertexInputAttrDescVector;
  mVertexData.layoutBindingVector = descriptorLayoutBindingVector;

  createShaderModule(deps.vertexPath, deps.device, &mVertexData.handle);
  createShaderModule(deps.fragmentPath, deps.device, &mFragmentData.handle);

  VkPipelineShaderStageCreateInfo vertexStage{};
  vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexStage.pNext = nullptr;
  vertexStage.flags = 0;
  vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexStage.module = mVertexData.handle;
  vertexStage.pName = "main";
  vertexStage.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo fragmentStage{};
  fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentStage.pNext = nullptr;
  fragmentStage.flags = 0;
  fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentStage.module = mFragmentData.handle;
  fragmentStage.pName = "main";
  fragmentStage.pSpecializationInfo = nullptr;

  mData.shaderStagesVector = { vertexStage, fragmentStage };

  UDEBUG("Created shader modules for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FShaderModulesVulkan::close(VkDevice device) {
  UTRACE("Closing shader modules for graphics pipeline {}...", mData.logInfo);

  closeShaderModule(device, &mVertexData.handle, "vertex");
  closeShaderModule(device, &mFragmentData.handle, "fragment");
  mVertexData = {};
  mFragmentData = {};
  mData.shaderStagesVector.clear();
  mData = {};

  UDEBUG("Closed shader modules for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 createShaderModule(const char* path, VkDevice device, VkShaderModule* pOutShaderModule) {
  UTRACE("Creating shader module {} ...", path);

  std::vector<char> code{};
  b32 loadedFile{ FFileManager::loadFile(path, &code) };
  if (not loadedFile) {
    UERROR("Could not load shader {}!", path);
    return UFALSE;
  }

  VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const u32*>(code.data());;

  VkResult result{ vkCreateShaderModule(device, &createInfo, nullptr, pOutShaderModule) };
  if (result != VK_SUCCESS) {
    UERROR("Could not create shader module via create method! VkResult {}", result);
    return UFALSE;
  }

  UTRACE("Created shader module {}!", path);
  return UTRUE;
}


b32 closeShaderModule(VkDevice device, VkShaderModule* pShaderModule, const char* logInfo) {
  UTRACE("Closing {} shader module...", logInfo);

  if (*pShaderModule != VK_NULL_HANDLE) {
    UTRACE("Destroying {} shader module...", logInfo);
    vkDestroyShaderModule(device, *pShaderModule, nullptr);
    *pShaderModule = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} shader module is not created, it won't be destroyed!", logInfo);
  }

  UTRACE("Closed {} shader module!", logInfo);
  return UTRUE;
}


}
