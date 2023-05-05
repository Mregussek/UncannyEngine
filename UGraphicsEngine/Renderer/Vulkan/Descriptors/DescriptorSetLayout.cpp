
#include "DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FDescriptorSetLayout::~FDescriptorSetLayout()
{
  Destroy();
}


void FDescriptorSetLayout::AddBinding(u32 binding, VkDescriptorType type, u32 count, VkShaderStageFlags stageFlags,
                                      const VkSampler* pImmutableSamplers)
{
  m_Bindings.push_back({
    .binding = binding,
    .descriptorType = type,
    .descriptorCount = count,
    .stageFlags = stageFlags,
    .pImmutableSamplers = pImmutableSamplers
  });
}


void FDescriptorSetLayout::AddBinding(VkDescriptorSetLayoutBinding layoutBinding)
{
  m_Bindings.push_back(layoutBinding);
}


void FDescriptorSetLayout::Create(VkDevice vkDevice)
{
  m_Device = vkDevice;

  VkDescriptorSetLayoutCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .bindingCount = static_cast<u32>(m_Bindings.size()),
    .pBindings = m_Bindings.data()
  };

  VkResult result = vkCreateDescriptorSetLayout(m_Device, &createInfo, nullptr, &m_DescriptorSetLayout);
  AssertVkAndThrow(result);
}


void FDescriptorSetLayout::Destroy()
{
  if (m_DescriptorSetLayout != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    m_DescriptorSetLayout = VK_NULL_HANDLE;
  }
}


}
