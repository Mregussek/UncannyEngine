
#include "GraphicsPipelinesVulkan.h"
#include <utilities/Logger.h>
#include <renderer/Mesh.h>
#include <filesystem/FileManager.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <renderer/vulkan_renderer/resources/BufferVulkan.h>


namespace uncanny
{


static b32 createShaderModule(const char* path, VkDevice device, VkShaderModule* pOutShaderModule);
static b32 closeShaderModule(VkDevice device, VkShaderModule* pShaderModule, const char* logInfo);


b32 FGraphicsPipelineShaderModulesVulkan::create(
    const FGraphicsPipelineShaderModulesCreateDependenciesVulkan& deps) {
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


b32 FGraphicsPipelineShaderModulesVulkan::close(VkDevice device) {
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


b32 FGraphicsPipelineLayoutVulkan::create(
    const FGraphicsPipelineLayoutCreateDependenciesVulkan& deps) {
  UTRACE("Creating layout for graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.pNext = nullptr;
  descriptorSetLayoutCreateInfo.flags = 0;
  descriptorSetLayoutCreateInfo.bindingCount = deps.pVertexData->layoutBindingVector.size();
  descriptorSetLayoutCreateInfo.pBindings = deps.pVertexData->layoutBindingVector.data();

  U_VK_ASSERT( vkCreateDescriptorSetLayout(deps.device, &descriptorSetLayoutCreateInfo, nullptr,
                                           &(mData.descriptorSetLayout)) );

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &(mData.descriptorSetLayout);
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  U_VK_ASSERT( vkCreatePipelineLayout(deps.device, &pipelineLayoutCreateInfo, nullptr,
                                      &(mData.pipelineLayout)) );

  UDEBUG("Created layout for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FGraphicsPipelineLayoutVulkan::close(VkDevice device) {
  UTRACE("Closing layout for graphics pipeline {}...", mData.logInfo);

  if (mData.pipelineLayout != VK_NULL_HANDLE) {
    UTRACE("Destroying {} graphics pipeline layout...", mData.logInfo);
    vkDestroyPipelineLayout(device, mData.pipelineLayout, nullptr);
    mData.pipelineLayout = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} graphics pipeline layout was not created, it won't be destroyed!", mData.logInfo);
  }

  if (mData.descriptorSetLayout != VK_NULL_HANDLE) {
    UTRACE("Destroying {} descriptor set layout...", mData.logInfo);
    vkDestroyDescriptorSetLayout(device, mData.descriptorSetLayout, nullptr);
    mData.descriptorSetLayout = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} descriptor set layout was not created, it won't be destroyed!", mData.logInfo);
  }

  UDEBUG("Closed layout for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FGraphicsPipelineDescriptorsVulkan::create(
    const FGraphicsPipelineDescriptorsCreateDependenciesVulkan& deps) {
  UTRACE("Creating descriptors for graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

  if (
      deps.pVertexShaderData->layoutBindingVector.size() != 1 or
      deps.pVertexShaderData->layoutBindingVector[0].descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
    UERROR("Wrong descriptor layout binding size or type!");
    return UFALSE;
  }

  VkDescriptorPoolSize poolSize{};
  poolSize.type = deps.pVertexShaderData->layoutBindingVector[0].descriptorType;
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
  allocateInfo.pSetLayouts = &(deps.pLayoutData->descriptorSetLayout);

  mData.descriptorSetVector.resize(allocateInfo.descriptorSetCount);
  U_VK_ASSERT( vkAllocateDescriptorSets(deps.device, &allocateInfo,
                                        mData.descriptorSetVector.data()) );

  UDEBUG("Created descriptors for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FGraphicsPipelineDescriptorsVulkan::close(VkDevice device) {
  UTRACE("Closing descriptors for graphics pipeline {}...", mData.logInfo);

  mData.descriptorSetVector.clear();

  if (mData.pool != VK_NULL_HANDLE) {
    UTRACE("Destroying descriptor pool...");
    vkDestroyDescriptorPool(device, mData.pool, nullptr);
    mData.pool = VK_NULL_HANDLE;
  }
  else {
    UWARN("Descriptor pool is not created, so it won't be closed!");
  }

  UDEBUG("Closed descriptors for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FGraphicsPipelineVulkan::create(const FGraphicsPipelineCreateDependenciesVulkan& deps) {
  UTRACE("Creating graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

  mShaders.create(*deps.pShaderDeps);

  FGraphicsPipelineLayoutCreateDependenciesVulkan layoutCreateDeps{};
  layoutCreateDeps.device = deps.device;
  layoutCreateDeps.pVertexData = &mShaders.getVertexData();
  layoutCreateDeps.pFragmentData = &mShaders.getFragmentData();
  layoutCreateDeps.logInfo = deps.logInfo;

  mLayout.create(layoutCreateDeps);

  FGraphicsPipelineDescriptorsCreateDependenciesVulkan descriptorsCreateDeps{};
  descriptorsCreateDeps.device = deps.device;
  descriptorsCreateDeps.pVertexShaderData = &mShaders.getVertexData();
  descriptorsCreateDeps.pLayoutData = &mLayout.getData();
  descriptorsCreateDeps.logInfo = deps.logInfo;

  mDescriptors.create(descriptorsCreateDeps);

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = nullptr;
  vertexInputStateCreateInfo.flags = 0;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.pVertexBindingDescriptions =
      &mShaders.getVertexData().inputBindingDescription;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      mShaders.getVertexData().inputAttrDescVector.size();
  vertexInputStateCreateInfo.pVertexAttributeDescriptions =
      mShaders.getVertexData().inputAttrDescVector.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.pNext = nullptr;
  inputAssemblyStateCreateInfo.flags = 0;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = nullptr;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = nullptr; // will be handled via vkCmdSetViewport
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = nullptr; // will be handled via vkCmdSetScissor

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
  rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationStateCreateInfo.pNext = nullptr;
  rasterizationStateCreateInfo.flags = 0;
  rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
  rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
  rasterizationStateCreateInfo.depthBiasClamp = 0.f;
  rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.f;
  rasterizationStateCreateInfo.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
  multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateCreateInfo.pNext = nullptr;
  multisampleStateCreateInfo.flags = 0;
  multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleStateCreateInfo.minSampleShading = 0.f;
  multisampleStateCreateInfo.pSampleMask = nullptr;
  multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
  colorBlendAttachmentState.blendEnable = VK_FALSE;
  colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.pNext = nullptr;
  colorBlendStateCreateInfo.flags = 0;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
  colorBlendStateCreateInfo.blendConstants[0] = 0.f;
  colorBlendStateCreateInfo.blendConstants[1] = 0.f;
  colorBlendStateCreateInfo.blendConstants[2] = 0.f;
  colorBlendStateCreateInfo.blendConstants[3] = 0.f;

  std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.pNext = nullptr;
  dynamicStateCreateInfo.flags = 0;
  dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
  dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

  VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
  depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilStateCreateInfo.pNext = nullptr;
  depthStencilStateCreateInfo.flags = 0;
  depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.front = {};
  depthStencilStateCreateInfo.back = {};
  depthStencilStateCreateInfo.minDepthBounds = 0.f;
  depthStencilStateCreateInfo.maxDepthBounds = 1.f;

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
  graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineCreateInfo.pNext = nullptr;
  graphicsPipelineCreateInfo.flags = 0;
  graphicsPipelineCreateInfo.stageCount = mShaders.getData().shaderStagesVector.size();
  graphicsPipelineCreateInfo.pStages = mShaders.getData().shaderStagesVector.data();
  graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
  graphicsPipelineCreateInfo.pTessellationState = nullptr;
  graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
  graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
  graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
  graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
  graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
  graphicsPipelineCreateInfo.layout = mLayout.getData().pipelineLayout;
  graphicsPipelineCreateInfo.renderPass = deps.renderPass;
  graphicsPipelineCreateInfo.subpass = 0;
  graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  graphicsPipelineCreateInfo.basePipelineIndex = 0;

  VkResult createdPipeline{ vkCreateGraphicsPipelines(deps.device, VK_NULL_HANDLE, 1,
                                                      &graphicsPipelineCreateInfo, nullptr,
                                                      &(mData.pipeline)) };
  if (createdPipeline != VK_SUCCESS) {
    UERROR("Could not create pipeline via vkCreateGraphicsPipelines, result: {}",
           createdPipeline);
    return UFALSE;
  }

  UDEBUG("Created graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FGraphicsPipelineVulkan::close(VkDevice device) {
  UTRACE("Closing graphics pipeline {}...", mData.logInfo);

  if (mData.pipeline != VK_NULL_HANDLE) {
    UTRACE("Destroying {} graphics pipeline...", mData.logInfo);
    vkDestroyPipeline(device, mData.pipeline, nullptr);
  }
  else {
    UWARN("As {} graphics pipeline was not created, it won't be destroyed!", mData.logInfo);
  }

  mLayout.close(device);
  mDescriptors.close(device);
  mShaders.close(device);

  UDEBUG("Closed graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


void FGraphicsPipelineVulkan::passCameraUboToDescriptor(VkDevice device,
                                                        FBufferVulkan* pCameraUBO) {
  UTRACE("Passing camera uniform data into descriptor from graphics pipeline {}...", mData.logInfo);

  VkDescriptorBufferInfo bufferInfo{ pCameraUBO->getDescriptorBufferInfo() };

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.pNext = nullptr;
  writeDescriptorSet.dstSet = mDescriptors.getData().descriptorSetVector[0];
  writeDescriptorSet.dstBinding = 0;
  writeDescriptorSet.dstArrayElement = 0;
  writeDescriptorSet.descriptorCount = 1;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writeDescriptorSet.pImageInfo = nullptr;
  writeDescriptorSet.pBufferInfo = &bufferInfo;
  writeDescriptorSet.pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);

  UDEBUG("Passed camera uniform data into descriptor from graphics pipeline {}!", mData.logInfo);
}


}
