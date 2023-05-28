
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FDescriptorPool::~FDescriptorPool()
{
  Destroy();
}


void FDescriptorPool::Create(VkDevice vkDevice, const FDescriptorSetLayout* pSetLayout, u32 maxSetsCount)
{
  m_Device = vkDevice;
  m_pSetLayout = pSetLayout;

  const auto& bindings = m_pSetLayout->GetBindings();

  std::vector<VkDescriptorPoolSize> poolSizes{};
  poolSizes.reserve(bindings.size());
  for (const VkDescriptorSetLayoutBinding& binding : bindings)
  {
    poolSizes.push_back(VkDescriptorPoolSize{
        .type = binding.descriptorType,
        .descriptorCount = binding.descriptorCount
    });
  }

  VkDescriptorPoolCreateInfo descriptorPoolInfo{
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .maxSets = maxSetsCount,
    .poolSizeCount = static_cast<u32>(poolSizes.size()),
    .pPoolSizes = poolSizes.data()
  };

  VkResult result = vkCreateDescriptorPool(m_Device, &descriptorPoolInfo, nullptr, &m_DescriptorPool);
  AssertVkAndThrow(result);
}


void FDescriptorPool::Destroy()
{
  if (m_DescriptorPool != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
    m_DescriptorPool = VK_NULL_HANDLE;
  }
}


void FDescriptorPool::AllocateDescriptorSet()
{
  VkDescriptorSetLayout setLayout = m_pSetLayout->GetHandle();
  VkDescriptorSetAllocateInfo allocateInfo{
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .pNext = nullptr,
    .descriptorPool = m_DescriptorPool,
    .descriptorSetCount = 1,
    .pSetLayouts = &setLayout
  };

  VkResult result = vkAllocateDescriptorSets(m_Device, &allocateInfo, &m_DescriptorSet);
  AssertVkAndThrow(result);
}


void FDescriptorPool::WriteTopLevelAsToDescriptorSet(VkAccelerationStructureKHR topLevelAS, u32 dstBinding) const
{
  VkWriteDescriptorSetAccelerationStructureKHR writeStructure{
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
    .pNext = nullptr,
    .accelerationStructureCount = 1,
    .pAccelerationStructures = &topLevelAS
  };

  VkWriteDescriptorSet writeDescriptorSet{
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = &writeStructure,
    .dstSet = m_DescriptorSet,
    .dstBinding = dstBinding,
    .dstArrayElement = 0,
    .descriptorCount = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
    .pImageInfo = nullptr,
    .pBufferInfo = nullptr,
    .pTexelBufferView = nullptr
  };

  vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
}


void FDescriptorPool::WriteStorageImageToDescriptorSet(VkImageView storageView, u32 dstBinding) const
{
  VkDescriptorImageInfo writeImage{
      .sampler = VK_NULL_HANDLE,
      .imageView = storageView,
      .imageLayout = VK_IMAGE_LAYOUT_GENERAL
  };

  VkWriteDescriptorSet writeDescriptorSet{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = m_DescriptorSet,
      .dstBinding = dstBinding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
      .pImageInfo = &writeImage,
      .pBufferInfo = nullptr,
      .pTexelBufferView = nullptr
  };

  vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
}


void FDescriptorPool::WriteSamplerToDescriptorSet(VkSampler sampler, VkImageView view, VkImageLayout layout,
                                                  u32 dstBinding)
{
  VkDescriptorImageInfo writeImage{
      .sampler = sampler,
      .imageView = view,
      .imageLayout = layout
  };

  VkWriteDescriptorSet writeDescriptorSet{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = m_DescriptorSet,
      .dstBinding = dstBinding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = &writeImage,
      .pBufferInfo = nullptr,
      .pTexelBufferView = nullptr
  };

  vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
}


void FDescriptorPool::WriteBufferToDescriptorSet(VkBuffer buffer, VkDeviceSize range, u32 dstBinding,
                                                 VkDescriptorType descriptorBufferType) const
{
  VkDescriptorBufferInfo writeBuffer{
    .buffer = buffer,
    .offset = 0,
    .range = range
  };

  VkWriteDescriptorSet writeDescriptorSet{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = m_DescriptorSet,
      .dstBinding = dstBinding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = descriptorBufferType,
      .pImageInfo = nullptr,
      .pBufferInfo = &writeBuffer,
      .pTexelBufferView = nullptr
  };

  vkUpdateDescriptorSets(m_Device, 1, &writeDescriptorSet, 0, nullptr);
}


}
