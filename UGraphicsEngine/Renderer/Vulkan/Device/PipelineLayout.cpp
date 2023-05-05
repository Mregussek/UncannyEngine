
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
  m_Device = vkDevice;

  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = (u32)setLayouts.size(),
      .pSetLayouts = setLayouts.data(),
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr
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
