
#include "ShaderModulesVulkan.h"
#include <filesystem/FileManager.h>
#include <renderer/Mesh.h>
#include <utilities/Logger.h>
#include <renderer/vulkan_context/VulkanUtilities.h>


namespace uncanny
{


static b32 createShaderModule(const char* path, VkDevice device, VkShaderModule* pOutShaderModule);
static b32 closeShaderModule(VkDevice device, VkShaderModule* pShaderModule, const char* logInfo);


b32 FShaderModulesVulkan::create(const FShaderModulesCreateDependenciesVulkan& deps) {
  UTRACE("Creating shader modules for graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

  VkDescriptorSetLayoutBinding cameraDescriptorLayoutBinding;
  cameraDescriptorLayoutBinding.binding = 0;
  cameraDescriptorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  cameraDescriptorLayoutBinding.descriptorCount = 1;
  cameraDescriptorLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  cameraDescriptorLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.pNext = nullptr;
  descriptorSetLayoutCreateInfo.flags = 0;
  descriptorSetLayoutCreateInfo.bindingCount = 1;
  descriptorSetLayoutCreateInfo.pBindings = &cameraDescriptorLayoutBinding;

  U_VK_ASSERT( vkCreateDescriptorSetLayout(deps.device, &descriptorSetLayoutCreateInfo, nullptr,
                                           &(mData.descriptorSetLayout)) );

  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
  poolCreateInfo.pNext = nullptr;
  poolCreateInfo.flags = 0;
  poolCreateInfo.maxSets = 1;
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.pPoolSizes = &poolSize;

  U_VK_ASSERT( vkCreateDescriptorPool(deps.device, &poolCreateInfo, nullptr, &(mData.pool)) );

  VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.descriptorPool = mData.pool;
  allocateInfo.descriptorSetCount = 1;
  allocateInfo.pSetLayouts = &(mData.descriptorSetLayout);

  U_VK_ASSERT( vkAllocateDescriptorSets(deps.device, &allocateInfo,
                                        &mData.cameraDescriptorSet) );

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
  mVertexData.cameraDescriptorLayoutBinding = cameraDescriptorLayoutBinding;

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

  if (mData.descriptorSetLayout != VK_NULL_HANDLE) {
    UTRACE("Destroying {} descriptor set layout...", mData.logInfo);
    vkDestroyDescriptorSetLayout(device, mData.descriptorSetLayout, nullptr);
    mData.descriptorSetLayout = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} descriptor set layout was not created, it won't be destroyed!", mData.logInfo);
  }

  if (mData.pool != VK_NULL_HANDLE) {
    UTRACE("Destroying descriptor pool...");
    vkDestroyDescriptorPool(device, mData.pool, nullptr);
    mData.pool = VK_NULL_HANDLE;
  }
  else {
    UWARN("Descriptor pool is not created, so it won't be closed!");
  }

  mData.cameraDescriptorSet = VK_NULL_HANDLE;

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
