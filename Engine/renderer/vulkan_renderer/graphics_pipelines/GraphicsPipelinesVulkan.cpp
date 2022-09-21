
#include "GraphicsPipelinesVulkan.h"
#include <utilities/Logger.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <renderer/vulkan_renderer/resources/BufferVulkan.h>
#include <renderer/vulkan_renderer/resources/ImageVulkan.h>


namespace uncanny
{


b32 FGraphicsPipelineVulkan::create(const FGraphicsPipelineCreateDependenciesVulkan& deps) {
  UTRACE("Creating graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

  mRenderPass.create(*deps.pRenderPassDeps);
  mShaders.create(*deps.pShaderDeps);

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &(mShaders.getData().descriptorSetLayout);
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  U_VK_ASSERT( vkCreatePipelineLayout(deps.device, &pipelineLayoutCreateInfo, nullptr,
                                      &(mData.pipelineLayout)) );

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
  graphicsPipelineCreateInfo.layout = mData.pipelineLayout;
  graphicsPipelineCreateInfo.renderPass = mRenderPass.getData().renderPass;
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

  if (mData.pipelineLayout != VK_NULL_HANDLE) {
    UTRACE("Destroying {} graphics pipeline layout...", mData.logInfo);
    vkDestroyPipelineLayout(device, mData.pipelineLayout, nullptr);
    mData.pipelineLayout = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} graphics pipeline layout was not created, it won't be destroyed!", mData.logInfo);
  }

  mShaders.close(device);
  mRenderPass.close(device);

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
  writeDescriptorSet.dstSet = mShaders.getData().cameraDescriptorSet;
  writeDescriptorSet.dstBinding = mShaders.getVertexData().cameraDescriptorLayoutBinding.binding;
  writeDescriptorSet.dstArrayElement = 0;
  writeDescriptorSet.descriptorCount = 1;
  writeDescriptorSet.descriptorType =
      mShaders.getVertexData().cameraDescriptorLayoutBinding.descriptorType;
  writeDescriptorSet.pImageInfo = nullptr;
  writeDescriptorSet.pBufferInfo = &bufferInfo;
  writeDescriptorSet.pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);

  UDEBUG("Passed camera uniform data into descriptor from graphics pipeline {}!", mData.logInfo);
}


b32 FGraphicsPipelineVulkan::recordUsageCommands(
    const FGraphicsPipelineRecordCommandsDependencies& deps) {
  UTRACE("Recording command buffers with indexed vertex buffer binding in graphics pipeline for"
         "render target images...");

  VkExtent2D imageExtent{
      deps.pRenderTargets->at(0).getData().extent.width,
      deps.pRenderTargets->at(0).getData().extent.height
  };

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = 0;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkRect2D renderArea{};
  renderArea.extent = imageExtent;
  renderArea.offset = { 0, 0 };

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

  VkClearValue clearColorValue{};
  clearColorValue.color = { 0.2f, 0.5f, 0.8f, 0.0f };
  VkClearValue clearDepthValue{};
  clearDepthValue.depthStencil = { 1.f, 0 };
  std::array<VkClearValue, 2> clearValues{ clearColorValue, clearDepthValue };

  VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  renderPassBeginInfo.pNext = nullptr;
  renderPassBeginInfo.renderPass = mRenderPass.getData().renderPass;
  renderPassBeginInfo.framebuffer = VK_NULL_HANDLE; // will be filled later
  renderPassBeginInfo.renderArea = {}; // will be filled later
  renderPassBeginInfo.clearValueCount = clearValues.size();
  renderPassBeginInfo.pClearValues = clearValues.data();

  VkDeviceSize vertexBufferOffsets[]{ 0 };
  VkBuffer vertexHandle{ deps.pVertexBuffer->getData().handle };
  VkBuffer indexHandle{ deps.pIndexBuffer->getData().handle };
  u32 indicesCount{ deps.pIndexBuffer->getData().elemCount };
  u32 renderTargetsSize{ (u32)deps.pRenderTargets->size() };
  std::vector<VkDescriptorSet> descriptorSets{ mShaders.getData().cameraDescriptorSet };

  for (u32 i = 0; i < renderTargetsSize; i++) {
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.framebuffer = deps.pRenderTargets->at(i).getData().handleFramebuffer;

    VkCommandBuffer cmdBuffer{ deps.pCommandBuffers->at(i) };

    VkResult properlyBegan{ vkBeginCommandBuffer(cmdBuffer, &commandBufferBeginInfo) };
    if (properlyBegan != VK_SUCCESS) {
      UERROR("Cannot record any commands! Wrong output of vkBeginCommandBuffer!");
      return UFALSE;
    }

    vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mData.pipeline);
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexHandle, vertexBufferOffsets);
    vkCmdBindIndexBuffer(cmdBuffer, indexHandle, 0, VK_INDEX_TYPE_UINT32);
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            mData.pipelineLayout, 0,
                            descriptorSets.size(), descriptorSets.data(),
                            0, nullptr);
    vkCmdDrawIndexed(cmdBuffer, indicesCount, 1, 0, 0, 0);

    vkCmdEndRenderPass(cmdBuffer);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(cmdBuffer) };
    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  UDEBUG("Recorded command buffers with indexed vertex buffer binding in graphics pipeline for "
         "render target images!");
  return UTRUE;
}


}
