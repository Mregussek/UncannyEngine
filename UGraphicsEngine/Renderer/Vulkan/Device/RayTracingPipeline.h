
#ifndef UNCANNYENGINE_RAYTRACINGPIPELINE_H
#define UNCANNYENGINE_RAYTRACINGPIPELINE_H


#include <volk.h>
#include "UTools/Filesystem/Path.h"


namespace uncanny::vulkan
{


class FGLSLShaderCompiler;
class FPipelineLayout;


struct FRayTracingPipelineSpecification
{
  FPath rayClosestHitPath{ "" };
  FPath rayGenerationPath{ "" };
  FPath rayMissPath{ "" };
  FGLSLShaderCompiler* pGlslCompiler{ nullptr };
  FPipelineLayout* pPipelineLayout{};
};


class FRayTracingPipeline
{

  friend class FRenderDeviceFactory;

public:

  FRayTracingPipeline() = default;

  void Create(const FRayTracingPipelineSpecification& specification);

  void Destroy();

private:

  explicit FRayTracingPipeline(VkDevice vkDevice);


  VkPipeline m_Pipeline{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RAYTRACINGPIPELINE_H
