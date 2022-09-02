
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createDescriptors() {
  UTRACE("Creating descriptors...");

  u32 maxDescriptorCount{ mMaxFramesInFlight };

  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = maxDescriptorCount;

  VkDescriptorPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
  poolCreateInfo.pNext = nullptr;
  poolCreateInfo.flags = 0;
  poolCreateInfo.maxSets = maxDescriptorCount;
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.pPoolSizes = &poolSize;

  U_VK_ASSERT( vkCreateDescriptorPool(mContextPtr->Device(), &poolCreateInfo, nullptr,
                                      &mVkDescriptorPool) );

  std::vector<VkDescriptorSetLayout> layouts(maxDescriptorCount, mVkDescriptorSetLayoutMeshColor);

  VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.descriptorPool = mVkDescriptorPool;
  allocateInfo.descriptorSetCount = layouts.size();
  allocateInfo.pSetLayouts = layouts.data();

  mVkDescriptorSets.resize(allocateInfo.descriptorSetCount);
  U_VK_ASSERT( vkAllocateDescriptorSets(mContextPtr->Device(), &allocateInfo,
                                        mVkDescriptorSets.data()) );

  if (mVkDescriptorSets.size() != mUniformBufferCameraVector.size()) {
    UERROR("Wrong size of descriptors sets and camera UBOs! ds {} cu {}",
           mVkDescriptorSets.size(), mUniformBufferCameraVector.size());
    return UFALSE;
  }

  for (size_t i = 0; i < mVkDescriptorSets.size(); i++) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = mUniformBufferCameraVector[i].handle;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(FCameraUBO);

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = nullptr;
    writeDescriptorSet.dstSet = mVkDescriptorSets[i];
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pImageInfo = nullptr;
    writeDescriptorSet.pBufferInfo = &bufferInfo;
    writeDescriptorSet.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(mContextPtr->Device(), 1, &writeDescriptorSet, 0, nullptr);
  }

  UDEBUG("Created descriptors!");
  return UTRUE;
}


b32 FRendererVulkan::closeDescriptors() {
  UTRACE("Closing descriptors...");

  //if (mVkDescriptorSets.empty()) {
  //  UWARN("As descriptors sets array is empty, it won't be freed!");
  //}
  //else {
  //  UTRACE("Freeing {} descriptor sets...", mVkDescriptorSets.size());
  //  vkFreeDescriptorSets(mContextPtr->Device(), mVkDescriptorPool, mVkDescriptorSets.size(),
  //                       mVkDescriptorSets.data());
  //}

  if (mVkDescriptorPool != VK_NULL_HANDLE) {
    UTRACE("Destroying descriptor pool...");
    vkDestroyDescriptorPool(mContextPtr->Device(), mVkDescriptorPool, nullptr);
  }
  else {
    UWARN("Descriptor pool is not created, so it won't be closed!");
  }

  UDEBUG("Closed descriptors!");
  return UTRUE;
}


}
