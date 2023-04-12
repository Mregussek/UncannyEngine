
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include <algorithm>


namespace uncanny::vulkan
{


FDescriptorPool::FDescriptorPool(VkDevice vkDevice)
  : m_Device(vkDevice)
{
}


void FDescriptorPool::Create(const FDescriptorSetLayout* pSetLayout, u32 maxSetsCount)
{
  m_pSetLayout = pSetLayout;

  const auto& bindings = m_pSetLayout->GetBindings();

  std::vector<VkDescriptorPoolSize> poolSizes{};
  poolSizes.reserve(bindings.size());
  std::ranges::for_each(bindings, [&poolSizes, maxSetsCount](const VkDescriptorSetLayoutBinding& binding)
  {
    poolSizes.push_back(VkDescriptorPoolSize{
      .type = binding.descriptorType,
      .descriptorCount = binding.descriptorCount
    });
  });

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


void FDescriptorPool::WriteStorageImageToDescriptorSet(const FImage& image, u32 dstBinding) const
{
  VkDescriptorImageInfo writeImage{
      .sampler = VK_NULL_HANDLE,
      .imageView = image.GetHandleView(),
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


}
