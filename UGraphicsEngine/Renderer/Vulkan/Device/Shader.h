
#ifndef UNCANNYENGINE_SHADER_H
#define UNCANNYENGINE_SHADER_H


#include <volk.h>
#include <UTools/UTypes.h>
#include "GlslShaderCompiler.h"
#include "UTools/Filesystem/Path.h"


namespace uncanny::vulkan
{


/// @brief FShader is a wrapper for VkShaderModule.
/// With this class you can parse, compile and create shader modules.
class FShader
{
public:

  FShader() = default;

  FShader(const FShader& other) = delete;
  FShader(FShader&& other) = delete;

  FShader& operator=(const FShader& other) = delete;
  FShader& operator=(FShader&& other) = delete;

  ~FShader();

public:

  /// @brief Depending on given path, method reads binary shader code and creates VkShaderModule or
  /// reads GLSL shader code, compiles it and creates VkShaderModule in given shaderModule variable.
  /// @param shaderModule stores newly created VkShaderModule.
  /// @param path to shader file, can .spv or GLSL source code.
  /// @param stage necessary for GLSL shader compilation.
  static void ParseAndCreateModule(FShader& shaderModule, const FPath& path, EShaderCompilerStage stage,
                                   const FGLSLShaderCompiler* pGlslCompiler, VkDevice vkDevice);

  void Create(const u32* pSpvSource, u32 codeSize, VkDevice vkDevice);

  void Destroy();

// Getters
public:

  [[nodiscard]] VkShaderModule GetHandle() const { return m_ShaderModule; }

// Members
private:

  VkShaderModule m_ShaderModule{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SHADER_H
