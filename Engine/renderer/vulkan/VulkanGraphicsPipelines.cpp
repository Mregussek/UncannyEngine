
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>
#include <filesystem/FileManager.h>


namespace uncanny
{


struct FShaderModulesVulkan {
  VkShaderModule vertex{ VK_NULL_HANDLE };
  VkShaderModule fragment{ VK_NULL_HANDLE };
};


static b32 createTriangleGraphicsPipeline(VkDevice device, VkExtent3D renderTargetExtent,
                                          VkRenderPass renderPass,
                                          const FShaderModulesVulkan& shaderModules,
                                          VkPipelineLayout* pOutPipelineLayout,
                                          VkPipeline* pOutPipeline);


static b32 createShaderModule(const char* path, VkDevice device, VkShaderModule* pOutShaderModule);


static b32 closeShaderModule(VkDevice device, VkShaderModule* pShaderModule, const char* logInfo);


b32 FRenderContextVulkan::createGraphicsPipelinesGeneral() {
  UTRACE("Creating graphics pipelines general...");

  FShaderModulesVulkan shaderModules{};

  const char* vertexPath{ "shaders/triangle.vert.spv" };
  b32 createdVertexModule{ createShaderModule(vertexPath, mVkDevice, &shaderModules.vertex) };
  if (not createdVertexModule) {
    UERROR("Could not create vertex shader module from path {}!", vertexPath);
    return UFALSE;
  }
  const char* fragmentPath{ "shaders/triangle.frag.spv" };
  b32 createdFragmentModule{ createShaderModule(fragmentPath, mVkDevice, &shaderModules.fragment) };
  if (not createdFragmentModule) {
    UERROR("Could not create fragment shader module from path {}!", fragmentPath);
    return UFALSE;
  }

  // TODO: handle taking proper extent properly for viewport and scissors!
  UWARN("During graphics pipeline creation using 0-indexes render target extent!");

  b32 createdTrianglePipeline{ createTriangleGraphicsPipeline(mVkDevice,
                                                              mImageRenderTargetVector[0].extent,
                                                              mVkRenderPass, shaderModules,
                                                              &mVkPipelineLayoutTriangle,
                                                              &mVkPipelineTriangle) };
  // We want to clean shader modules independently of result
  closeShaderModule(mVkDevice, &shaderModules.vertex, "vertex");
  closeShaderModule(mVkDevice, &shaderModules.fragment, "fragment");

  if (not createdTrianglePipeline) {
    UERROR("Could not create triangle graphics pipeline!");
    return UFALSE;
  }

  UDEBUG("Created graphics pipelines general!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsPipelinesGeneral() {
  UTRACE("Closing graphics pipelines general...");

  if (mVkPipelineTriangle != VK_NULL_HANDLE) {
    UTRACE("Destroying triangle graphics pipeline...");
    vkDestroyPipeline(mVkDevice, mVkPipelineTriangle, nullptr);
  }
  else {
    UWARN("As triangle graphics pipeline was not created, it won't be destroyed!");
  }

  if (mVkPipelineLayoutTriangle != VK_NULL_HANDLE) {
    UTRACE("Destroying triangle graphics pipeline layout...");
    vkDestroyPipelineLayout(mVkDevice, mVkPipelineLayoutTriangle, nullptr);
  }
  else {
    UWARN("As triangle graphics pipeline layout was not created, it won't be destroyed!");
  }

  UDEBUG("Closed graphics pipelines general!");
  return UTRUE;
}


b32 createTriangleGraphicsPipeline(VkDevice device, VkExtent3D renderTargetExtent,
                                   VkRenderPass renderPass,
                                   const FShaderModulesVulkan& shaderModules,
                                   VkPipelineLayout* pOutPipelineLayout, VkPipeline* pOutPipeline) {
  UTRACE("Creating triangle graphics pipeline...");

  VkPipelineShaderStageCreateInfo vertexStageCreateInfo{};
  vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexStageCreateInfo.pNext = nullptr;
  vertexStageCreateInfo.flags = 0;
  vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexStageCreateInfo.module = shaderModules.vertex;
  vertexStageCreateInfo.pName = "main";
  vertexStageCreateInfo.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo fragmentStageCreateInfo{};
  fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentStageCreateInfo.pNext = nullptr;
  fragmentStageCreateInfo.flags = 0;
  fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentStageCreateInfo.module = shaderModules.fragment;
  fragmentStageCreateInfo.pName = "main";
  fragmentStageCreateInfo.pSpecializationInfo = nullptr;

  constexpr u32 shaderStagesCount{ 2 };
  VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[shaderStagesCount]{
    vertexStageCreateInfo, fragmentStageCreateInfo };

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = nullptr;
  vertexInputStateCreateInfo.flags = 0;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.pNext = nullptr;
  inputAssemblyStateCreateInfo.flags = 0;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.width = (f32)renderTargetExtent.width;
  viewport.height = (f32)renderTargetExtent.height;
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;

  VkRect2D scissor{};
  scissor.extent = { renderTargetExtent.width, renderTargetExtent.height };
  scissor.offset = { 0, 0 };

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = nullptr;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = &scissor;

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

  constexpr u32 dynamicStatesCount{ 2 };
  VkDynamicState dynamicStates[dynamicStatesCount]{ VK_DYNAMIC_STATE_VIEWPORT,
                                                    VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.pNext = nullptr;
  dynamicStateCreateInfo.flags = 0;
  dynamicStateCreateInfo.dynamicStateCount = dynamicStatesCount;
  dynamicStateCreateInfo.pDynamicStates = dynamicStates;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 0;
  pipelineLayoutCreateInfo.pSetLayouts = nullptr;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  VkResult createdPipelineLayout{ vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo,
                                                         nullptr, pOutPipelineLayout) };
  if (createdPipelineLayout != VK_SUCCESS) {
    UERROR("Could not create pipeline layout!");
    return UFALSE;
  }

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
  graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineCreateInfo.pNext = nullptr;
  graphicsPipelineCreateInfo.flags = 0;
  graphicsPipelineCreateInfo.stageCount = shaderStagesCount;
  graphicsPipelineCreateInfo.pStages = shaderStagesCreateInfo;
  graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
  graphicsPipelineCreateInfo.pTessellationState = nullptr;
  graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
  graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
  graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
  graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
  graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
  graphicsPipelineCreateInfo.layout = *pOutPipelineLayout;
  graphicsPipelineCreateInfo.renderPass = renderPass;
  graphicsPipelineCreateInfo.subpass = 0;
  graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  graphicsPipelineCreateInfo.basePipelineIndex = 0;

  VkResult createdPipeline{ vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                                      &graphicsPipelineCreateInfo, nullptr,
                                                      pOutPipeline) };
  if (createdPipeline != VK_SUCCESS) {
    UERROR("Could not create triangle pipeline via vkCreateGraphicsPipelines, result: {}",
           createdPipeline);
    return UFALSE;
  }

  UDEBUG("Created triangle graphics pipeline!");
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
