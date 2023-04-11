
#include "PipelineLayout.h"
#include "UTools/UTypes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FPipelineLayout::FPipelineLayout(VkDevice vkDevice)
  : m_Device(vkDevice)
{
}


void FPipelineLayout::Create(std::span<VkDescriptorSetLayout> setLayouts)
{
  VkPipelineLayoutCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .setLayoutCount = static_cast<u32>(setLayouts.size()),
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
  }
}


}
