
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

  friend class FRenderDeviceFactory;

public:

  std::vector<u32> Compile(const char* glslSource, EShaderCompilerStage stage) const;

private:

  FGLSLShaderCompiler(u32 targetVulkanVersion, b8 spirv14Supported);


  glslang_target_client_version_t m_TargetVulkanVersion{};
  glslang_target_language_version_t m_TargetSpirvVersion{};

};


}


#endif //UNCANNYENGINE_GLSLSHADERCOMPILER_H
