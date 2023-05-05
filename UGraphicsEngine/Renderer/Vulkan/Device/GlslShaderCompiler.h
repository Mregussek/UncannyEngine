
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

  ~FGLSLShaderCompiler();

  void Initialize(u32 targetVulkanVersion);
  void Close();

  std::vector<u32> Compile(const char* glslSource, EShaderCompilerStage stage) const;

private:

  void ParseVulkanVersion(u32 targetVulkanVersion);

private:

  glslang_target_client_version_t m_TargetVulkanVersion{};
  b8 m_Initialized{ UFALSE };

};


}


#endif //UNCANNYENGINE_GLSLSHADERCOMPILER_H
