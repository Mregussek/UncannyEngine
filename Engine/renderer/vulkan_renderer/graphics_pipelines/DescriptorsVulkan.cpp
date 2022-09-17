
#include "DescriptorsVulkan.h"
#include "LayoutVulkan.h"
#include "ShaderModulesVulkan.h"
#include <utilities/Logger.h>
#include <renderer/vulkan_context/VulkanUtilities.h>


namespace uncanny
{


b32 FGraphicsPipelineDescriptorsVulkan::create(
    const FGraphicsPipelineDescriptorsCreateDependenciesVulkan& deps) {
  UTRACE("Creating descriptors for graphics pipeline {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;

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
  allocateInfo.pSetLayouts = &(deps.pLayoutData->descriptorSetLayout);

  U_VK_ASSERT( vkAllocateDescriptorSets(deps.device, &allocateInfo,
                                        &mData.cameraDescriptorSet) );

  UDEBUG("Created descriptors for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


b32 FGraphicsPipelineDescriptorsVulkan::close(VkDevice device) {
  UTRACE("Closing descriptors for graphics pipeline {}...", mData.logInfo);

  if (mData.pool != VK_NULL_HANDLE) {
    UTRACE("Destroying descriptor pool...");
    vkDestroyDescriptorPool(device, mData.pool, nullptr);
    mData.pool = VK_NULL_HANDLE;
  }
  else {
    UWARN("Descriptor pool is not created, so it won't be closed!");
  }

  mData.cameraDescriptorSet = VK_NULL_HANDLE;

  UDEBUG("Closed descriptors for graphics pipeline {}!", mData.logInfo);
  return UTRUE;
}


}
