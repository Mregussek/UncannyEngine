
#include "Shader.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"
#include "UTools/Filesystem/File.h"


namespace uncanny::vulkan
{


FShader::~FShader()
{
  Destroy();
}


void FShader::Create(const u32* pSpvSource, u32 codeSize, VkDevice vkDevice)
{
  m_Device = vkDevice;

  VkShaderModuleCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .codeSize = codeSize,
    .pCode = pSpvSource
  };
  VkResult result = vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ShaderModule);
  AssertVkAndThrow(result);
}


void FShader::Destroy()
{
  if (m_ShaderModule != VK_NULL_HANDLE)
  {
    vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
    m_ShaderModule = VK_NULL_HANDLE;
  }
}


void FShader::ParseAndCreateModule(FShader& shaderModule, const FPath& path, EShaderCompilerStage stage,
                                   const FGLSLShaderCompiler* pGlslCompiler, VkDevice vkDevice)
{
  const char* shaderPath = path.GetString().c_str();
  UDEBUG("Loading shader: {}", shaderPath);

  if (FPath::HasExtension(path, ".spv"))
  {
    std::vector<char> spvSource;
    spvSource = FFile::ReadBinary(shaderPath);
    shaderModule.Create(reinterpret_cast<const u32*>(spvSource.data()), spvSource.size(), vkDevice);
  }
  else
  {
    std::vector<u32> spvSource;
    std::vector<char> glslSource = FFile::Read(shaderPath);
    spvSource = pGlslCompiler->Compile(glslSource.data(), stage);
    shaderModule.Create(spvSource.data(), sizeof(u32) * spvSource.size(), vkDevice);
  }
}


}
