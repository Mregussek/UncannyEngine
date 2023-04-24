
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

  ~FGLSLShaderCompiler();

  void Initialize();

  std::vector<u32> Compile(const char* glslSource, EShaderCompilerStage stage) const;

private:

  explicit FGLSLShaderCompiler(u32 targetVulkanVersion);


  glslang_target_client_version_t m_TargetVulkanVersion{};

};


}


#endif //UNCANNYENGINE_GLSLSHADERCOMPILER_H
