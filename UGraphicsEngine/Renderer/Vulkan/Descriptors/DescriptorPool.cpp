
#include "DescriptorPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include <algorithm>


namespace uncanny::vulkan
{


FDescriptorPool::FDescriptorPool(VkDevice vkDevice)
  : m_Device(vkDevice)
{
}


void FDescriptorPool::Create(const FDescriptorSetLayout& setLayout, u32 maxSetsCount)
{
  const auto& bindings = setLayout.GetBindings();

  std::vector<VkDescriptorPoolSize> poolSizes{};
  poolSizes.reserve(bindings.size());

  std::ranges::for_each(bindings, [&poolSizes](const VkDescriptorSetLayoutBinding& binding)
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


}
