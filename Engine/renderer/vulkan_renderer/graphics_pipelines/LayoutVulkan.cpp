
#include "LayoutVulkan.h"
#include "ShaderModulesVulkan.h"
#include <utilities/Logger.h>
#include <renderer/vulkan_context/VulkanUtilities.h>


namespace uncanny
{


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


}
