
#ifndef UNCANNYENGINE_SHADER_H
#define UNCANNYENGINE_SHADER_H


#include <volk.h>
#include <UTools/UTypes.h>
#include "GlslShaderCompiler.h"
#include "UTools/Filesystem/Path.h"


namespace uncanny::vulkan
{


class FShader
{
public:

  ~FShader();

  static void ParseAndCreateModule(FShader& shaderModule, const FPath& path, EShaderCompilerStage stage,
                                   const FGLSLShaderCompiler* pGlslCompiler, VkDevice vkDevice);

  void Create(const u32* pSpvSource, u32 codeSize, VkDevice vkDevice);

  void Destroy();

  [[nodiscard]] VkShaderModule GetHandle() const { return m_ShaderModule; }

private:

  VkShaderModule m_ShaderModule{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SHADER_H
