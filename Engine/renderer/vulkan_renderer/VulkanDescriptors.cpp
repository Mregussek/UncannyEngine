
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createDescriptors(FGraphicsPipelineVulkan* pPipeline) {
  UTRACE("Creating descriptors...");

  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
  poolCreateInfo.pNext = nullptr;
  poolCreateInfo.flags = 0;
  poolCreateInfo.maxSets = 1;
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.pPoolSizes = &poolSize;

  U_VK_ASSERT( vkCreateDescriptorPool(mContextPtr->Device(), &poolCreateInfo, nullptr,
                                      &(pPipeline->descriptorPool)) );

  VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.descriptorPool = pPipeline->descriptorPool;
  allocateInfo.descriptorSetCount = 1;
  allocateInfo.pSetLayouts = &mGraphicsPipeline.descriptorSetLayout;

  pPipeline->descriptorSetVector.resize(allocateInfo.descriptorSetCount);
  U_VK_ASSERT( vkAllocateDescriptorSets(mContextPtr->Device(), &allocateInfo,
                                        pPipeline->descriptorSetVector.data()) );

  VkDescriptorBufferInfo bufferInfo{ mUniformBufferCamera.getDescriptorBufferInfo() };

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.pNext = nullptr;
  writeDescriptorSet.dstSet = pPipeline->descriptorSetVector[0];
  writeDescriptorSet.dstBinding = 0;
  writeDescriptorSet.dstArrayElement = 0;
  writeDescriptorSet.descriptorCount = 1;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writeDescriptorSet.pImageInfo = nullptr;
  writeDescriptorSet.pBufferInfo = &bufferInfo;
  writeDescriptorSet.pTexelBufferView = nullptr;

  vkUpdateDescriptorSets(mContextPtr->Device(), 1, &writeDescriptorSet, 0, nullptr);

  UDEBUG("Created descriptors!");
  return UTRUE;
}


b32 FRendererVulkan::closeDescriptors(FGraphicsPipelineVulkan* pPipeline) {
  UTRACE("Closing descriptors...");

  pPipeline->descriptorSetVector.clear();

  if (pPipeline->descriptorPool != VK_NULL_HANDLE) {
    UTRACE("Destroying descriptor pool...");
    vkDestroyDescriptorPool(mContextPtr->Device(), pPipeline->descriptorPool, nullptr);
  }
  else {
    UWARN("Descriptor pool is not created, so it won't be closed!");
  }

  UDEBUG("Closed descriptors!");
  return UTRUE;
}


}
