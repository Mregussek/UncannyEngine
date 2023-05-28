
#include "Sampler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FSampler::FSampler(VkDevice vkDevice)
  : m_Device(vkDevice)
{
}


FSampler::~FSampler()
{
  Destroy();
}


void FSampler::Create()
{
  VkSamplerCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .magFilter = VK_FILTER_LINEAR,
    .minFilter = VK_FILTER_LINEAR,
    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .mipLodBias = 0.f,
    .anisotropyEnable = VK_FALSE,
    .maxAnisotropy = 1.f,
    .compareEnable = VK_FALSE,
    .compareOp = VK_COMPARE_OP_NEVER,
    .minLod = 0.f,
    .maxLod = 0.f,
    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    .unnormalizedCoordinates = VK_FALSE
  };

  VkResult result = vkCreateSampler(m_Device, &createInfo, nullptr, &m_Sampler);
  AssertVkAndThrow(result);
}


void FSampler::Destroy()
{
  if (m_Sampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(m_Device, m_Sampler, nullptr);
    m_Sampler = VK_NULL_HANDLE;
  }
}


}
