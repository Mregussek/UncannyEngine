
#include "Shader.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FShader::FShader(VkDevice vkDevice)
  : m_Device(vkDevice)
{
}


FShader::~FShader()
{
  Destroy();
}


void FShader::Create(const std::vector<u32>& spvSource)
{
  VkShaderModuleCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .codeSize = sizeof(spvSource[0]) * spvSource.size(),
    .pCode = spvSource.data()
  };
  VkResult result = vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ShaderModule);
  AssertVkAndThrow(result);
}


void FShader::Destroy()
{
  if (m_ShaderModule != VK_NULL_HANDLE)
  {
    vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
  }
}


}
