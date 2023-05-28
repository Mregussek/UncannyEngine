
#include "PipelineLayout.h"
#include "UTools/UTypes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FPipelineLayout::~FPipelineLayout()
{
  Destroy();
}


void FPipelineLayout::Create(VkDevice vkDevice, VkDescriptorSetLayout setLayout)
{
  std::span<VkDescriptorSetLayout> span(&setLayout, 1);
  Create(vkDevice, span);
}


void FPipelineLayout::Create(VkDevice vkDevice, std::span<VkDescriptorSetLayout> setLayouts)
{
  Create(vkDevice, setLayouts, {});
}


void FPipelineLayout::Create(VkDevice vkDevice, VkPushConstantRange pushConstantRange)
{
  std::span<VkPushConstantRange> span(&pushConstantRange, 1);
  Create(vkDevice, span);
}


void FPipelineLayout::Create(VkDevice vkDevice, std::span<VkPushConstantRange> pushConstantRanges)
{
  Create(vkDevice, {}, pushConstantRanges);
}


void FPipelineLayout::Create(VkDevice vkDevice, std::span<VkDescriptorSetLayout> setLayouts,
                             std::span<VkPushConstantRange> pushConstantRanges)
{
  m_Device = vkDevice;

  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = (u32)setLayouts.size(),
      .pSetLayouts = setLayouts.data(),
      .pushConstantRangeCount = (u32)pushConstantRanges.size(),
      .pPushConstantRanges = pushConstantRanges.data()
  };

  VkResult result = vkCreatePipelineLayout(m_Device, &createInfo, nullptr, &m_PipelineLayout);
  AssertVkAndThrow(result);
}


void FPipelineLayout::Destroy()
{
  if (m_PipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    m_PipelineLayout = VK_NULL_HANDLE;
  }
}


}
