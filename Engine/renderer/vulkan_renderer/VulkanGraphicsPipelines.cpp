
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>
#include <filesystem/FileManager.h>
#include <renderer/Mesh.h>


namespace uncanny
{


struct FShaderModulesVulkan {
  VkShaderModule vertex{ VK_NULL_HANDLE };
  VkShaderModule fragment{ VK_NULL_HANDLE };
};


struct FGraphicsPipelineConfiguration {
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
  VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  std::vector<VkDynamicState> dynamicStates{};
  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
};


static void fillGraphicsPipelineDefaultConfig(FGraphicsPipelineConfiguration* pConfig);


static b32 createColoredMeshGraphicsPipeline(
    VkDevice device, VkRenderPass renderPass, FShaderModulesVulkan& shaderModules,
    FGraphicsPipelineVulkan* pOutPipeline);


static b32 closeGraphicsPipeline(VkDevice device, FGraphicsPipelineVulkan* pOutPipeline,
                                 const char* logInfo);


static b32 createShaderModule(const char* path, VkDevice device, VkShaderModule* pOutShaderModule);


static b32 closeShaderModule(VkDevice device, VkShaderModule* pShaderModule, const char* logInfo);


b32 FRendererVulkan::collectViewportScissorInfo(FGraphicsPipelineVulkan* pPipeline) const {
  UTRACE("Collecting viewport and scissor info for graphics pipeline...");

  // TODO: handle taking proper extent properly for viewport and scissors!
  UWARN("During collection data for viewport and scissor using 0-indexes render target extent!");
  VkExtent3D imageExtent{ mImageRenderTargetVector[0].getData().extent };

  VkViewport viewport{};
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.width = (f32)imageExtent.width;
  viewport.height = (f32)imageExtent.height;
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;

  VkRect2D scissor{};
  scissor.extent = { imageExtent.width, imageExtent.height };
  scissor.offset = { 0, 0 };

  pPipeline->viewport = viewport;
  pPipeline->scissor = scissor;

  UTRACE("Collected viewport and scissor info for graphics pipeline!");
  return UTRUE;
}


b32 FRendererVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  FShaderModulesVulkan shaderModules{};

  b32 createdMeshColorPipeline{ createColoredMeshGraphicsPipeline(
      mContextPtr->Device(), mVkRenderPass, shaderModules, &mGraphicsPipeline) };
  // We want to clean shader modules independently of result
  closeShaderModule(mContextPtr->Device(), &shaderModules.vertex, "vertex");
  closeShaderModule(mContextPtr->Device(), &shaderModules.fragment, "fragment");
  if (not createdMeshColorPipeline) {
    UERROR("Could not create mesh color graphics pipeline!");
    return UFALSE;
  }

  UDEBUG("Created graphics pipelines general!");
  return UTRUE;
}


b32 FRendererVulkan::closeGraphicsPipelinesGeneral() {
  UTRACE("Closing graphics pipelines general...");

  closeGraphicsPipeline(mContextPtr->Device(), &mGraphicsPipeline, "mesh_color");

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


void fillGraphicsPipelineDefaultConfig(FGraphicsPipelineConfiguration* pConfig) {
  VkPipelineShaderStageCreateInfo vertexStageCreateInfo{};
  vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexStageCreateInfo.pNext = nullptr;
  vertexStageCreateInfo.flags = 0;
  vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexStageCreateInfo.module = VK_NULL_HANDLE; // will be filled during creation
  vertexStageCreateInfo.pName = "main";
  vertexStageCreateInfo.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo fragmentStageCreateInfo{};
  fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentStageCreateInfo.pNext = nullptr;
  fragmentStageCreateInfo.flags = 0;
  fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentStageCreateInfo.module = VK_NULL_HANDLE; // will be filled during creation;
  fragmentStageCreateInfo.pName = "main";
  fragmentStageCreateInfo.pSpecializationInfo = nullptr;

  std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo{vertexStageCreateInfo,
                                                                      fragmentStageCreateInfo };

  pConfig->shaderStages = shaderStagesCreateInfo;

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = nullptr;
  vertexInputStateCreateInfo.flags = 0;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

  pConfig->vertexInputStateCreateInfo = vertexInputStateCreateInfo;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.pNext = nullptr;
  inputAssemblyStateCreateInfo.flags = 0;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  pConfig->inputAssemblyStateCreateInfo = inputAssemblyStateCreateInfo;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = nullptr;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = nullptr; // will be handled via vkCmdSetViewport
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = nullptr; // will be handled via vkCmdSetScissor

  pConfig->viewportStateCreateInfo = viewportStateCreateInfo;

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

  pConfig->rasterizationStateCreateInfo = rasterizationStateCreateInfo;

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

  pConfig->multisampleStateCreateInfo = multisampleStateCreateInfo;

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

  pConfig->colorBlendAttachmentState = colorBlendAttachmentState;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.pNext = nullptr;
  colorBlendStateCreateInfo.flags = 0;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &(pConfig->colorBlendAttachmentState);
  colorBlendStateCreateInfo.blendConstants[0] = 0.f;
  colorBlendStateCreateInfo.blendConstants[1] = 0.f;
  colorBlendStateCreateInfo.blendConstants[2] = 0.f;
  colorBlendStateCreateInfo.blendConstants[3] = 0.f;

  pConfig->colorBlendStateCreateInfo = colorBlendStateCreateInfo;

  std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
  pConfig->dynamicStates = dynamicStates;

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.pNext = nullptr;
  dynamicStateCreateInfo.flags = 0;
  dynamicStateCreateInfo.dynamicStateCount = pConfig->dynamicStates.size();
  dynamicStateCreateInfo.pDynamicStates = pConfig->dynamicStates.data();

  pConfig->dynamicStateCreateInfo = dynamicStateCreateInfo;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 0;
  pipelineLayoutCreateInfo.pSetLayouts = nullptr;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  pConfig->pipelineLayoutCreateInfo = pipelineLayoutCreateInfo;
}


b32 createColoredMeshGraphicsPipeline(
    VkDevice device, VkRenderPass renderPass, FShaderModulesVulkan& shaderModules,
    FGraphicsPipelineVulkan* pOutPipeline) {
  UTRACE("Creating triangle graphics pipeline...");

  FGraphicsPipelineConfiguration defaultConfig{};
  fillGraphicsPipelineDefaultConfig(&defaultConfig);

  const char* vertexPath{ "shaders/colored_mesh.vert.spv" };
  b32 createdVertexModule{ createShaderModule(vertexPath, device, &shaderModules.vertex) };
  if (not createdVertexModule) {
    UERROR("Could not create vertex shader module from path {}!", vertexPath);
    return UFALSE;
  }
  const char* fragmentPath{ "shaders/colored_mesh.frag.spv" };
  b32 createdFragmentModule{ createShaderModule(fragmentPath, device, &shaderModules.fragment) };
  if (not createdFragmentModule) {
    UERROR("Could not create fragment shader module from path {}!", fragmentPath);
    return UFALSE;
  }

  // Define shader stages...
  defaultConfig.shaderStages[0].module = shaderModules.vertex;
  defaultConfig.shaderStages[1].module = shaderModules.fragment;

  // Define input for vertex shader from the vertex buffers...
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

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = nullptr;
  vertexInputStateCreateInfo.flags = 0;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputAttrDescVector.size();
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttrDescVector.data();

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

  // Create pipeline layout
  VkDescriptorSetLayoutBinding cameraLayoutBinding{};
  cameraLayoutBinding.binding = 0;
  cameraLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  cameraLayoutBinding.descriptorCount = 1;
  cameraLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  cameraLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.pNext = nullptr;
  descriptorSetLayoutCreateInfo.flags = 0;
  descriptorSetLayoutCreateInfo.bindingCount = 1;
  descriptorSetLayoutCreateInfo.pBindings = &cameraLayoutBinding;

  VkResult createdDescriptorSetLayout{
    vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr,
                                &(pOutPipeline->descriptorSetLayout)) };
  if (createdDescriptorSetLayout != VK_SUCCESS) {
    UERROR("Could not create descriptor set layout!");
    return UFALSE;
  }

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &(pOutPipeline->descriptorSetLayout);
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  VkResult createdPipelineLayout{
      vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr,
                             &(pOutPipeline->pipelineLayout)) };
  if (createdPipelineLayout != VK_SUCCESS) {
    UERROR("Could not create pipeline layout!");
    return UFALSE;
  }

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
  graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineCreateInfo.pNext = nullptr;
  graphicsPipelineCreateInfo.flags = 0;
  graphicsPipelineCreateInfo.stageCount = defaultConfig.shaderStages.size();
  graphicsPipelineCreateInfo.pStages = defaultConfig.shaderStages.data();
  graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  graphicsPipelineCreateInfo.pInputAssemblyState = &defaultConfig.inputAssemblyStateCreateInfo;
  graphicsPipelineCreateInfo.pTessellationState = nullptr;
  graphicsPipelineCreateInfo.pViewportState = &defaultConfig.viewportStateCreateInfo;
  graphicsPipelineCreateInfo.pRasterizationState = &defaultConfig.rasterizationStateCreateInfo;
  graphicsPipelineCreateInfo.pMultisampleState = &defaultConfig.multisampleStateCreateInfo;
  graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
  graphicsPipelineCreateInfo.pColorBlendState = &defaultConfig.colorBlendStateCreateInfo;
  graphicsPipelineCreateInfo.pDynamicState = &defaultConfig.dynamicStateCreateInfo;
  graphicsPipelineCreateInfo.layout = pOutPipeline->pipelineLayout;
  graphicsPipelineCreateInfo.renderPass = renderPass;
  graphicsPipelineCreateInfo.subpass = 0;
  graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  graphicsPipelineCreateInfo.basePipelineIndex = 0;

  VkResult createdPipeline{ vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                                      &graphicsPipelineCreateInfo, nullptr,
                                                      &(pOutPipeline->pipeline)) };
  if (createdPipeline != VK_SUCCESS) {
    UERROR("Could not create triangle pipeline via vkCreateGraphicsPipelines, result: {}",
           createdPipeline);
    return UFALSE;
  }

  UDEBUG("Created triangle graphics pipeline!");
  return UTRUE;
}


b32 closeGraphicsPipeline(VkDevice device, FGraphicsPipelineVulkan* pOutPipeline,
                          const char* logInfo) {
  if (pOutPipeline and pOutPipeline->pipeline != VK_NULL_HANDLE) {
    UTRACE("Destroying {} graphics pipeline...", logInfo);
    vkDestroyPipeline(device, pOutPipeline->pipeline, nullptr);
  }
  else {
    UWARN("As {} graphics pipeline was not created, it won't be destroyed!", logInfo);
  }

  if (pOutPipeline and pOutPipeline->pipelineLayout != VK_NULL_HANDLE) {
    UTRACE("Destroying {} graphics pipeline layout...", logInfo);
    vkDestroyPipelineLayout(device, pOutPipeline->pipelineLayout, nullptr);
  }
  else {
    UWARN("As {} graphics pipeline layout was not created, it won't be destroyed!", logInfo);
  }

  if (pOutPipeline and pOutPipeline->descriptorSetLayout != VK_NULL_HANDLE) {
    UTRACE("Destroying {} descriptor set layout...", logInfo);
    vkDestroyDescriptorSetLayout(device, pOutPipeline->descriptorSetLayout, nullptr);
  }
  else {
    UWARN("As {} descriptor set layout was not created, it won't be destroyed!", logInfo);
  }

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
