
#ifndef UNCANNYENGINE_GLSLSHADERCOMPILER_H
#define UNCANNYENGINE_GLSLSHADERCOMPILER_H


#include <vector>
#include "UTools/UTypes.h"
#include <glslang/Include/glslang_c_interface.h>


namespace uncanny::vulkan
{


enum class EShaderCompilerStage
{
  VERTEX, TESSCONTROL, TESSEVALUATION, GEOMETRY, FRAGMENT, COMPUTE,
  RAYGEN, INTERSECT, ANYHIT, CLOSESTHIT, MISS
};


class FGLSLShaderCompiler
{
public:

  FGLSLShaderCompiler() = default;

  FGLSLShaderCompiler(const FGLSLShaderCompiler& other) = delete;
  FGLSLShaderCompiler(FGLSLShaderCompiler&& other) = delete;

  FGLSLShaderCompiler& operator=(const FGLSLShaderCompiler& other) = delete;
  FGLSLShaderCompiler& operator=(FGLSLShaderCompiler&& other) = delete;

  ~FGLSLShaderCompiler();

public:

  /// @brief Initializes process for shader compiler.
  /// @param targetVulkanVersion Vulkan application version, should be returned by vkEnumerateInstanceVersion()
  ///   or can be retrieved from: m_RenderContext.GetInstance()->GetAttributes().GetFullVersion()
  void Initialize(u32 targetVulkanVersion);
  void Close();

  /// @brief Compile() should be called after Initialize() and before Close().
  std::vector<u32> Compile(const char* glslSource, EShaderCompilerStage stage) const;

private:

  glslang_target_client_version_t m_TargetVulkanVersion{};
  b8 m_Initialized{ UFALSE };

};


}


#endif //UNCANNYENGINE_GLSLSHADERCOMPILER_H
