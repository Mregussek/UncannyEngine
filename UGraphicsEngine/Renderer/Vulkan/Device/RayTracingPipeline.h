
#ifndef UNCANNYENGINE_RAYTRACINGPIPELINE_H
#define UNCANNYENGINE_RAYTRACINGPIPELINE_H


#include <volk.h>
#include "UTools/Filesystem/Path.h"


namespace uncanny::vulkan
{


struct FRayTracingPipelineSpecification
{
  FPath rayClosestHitPath{ "" };
  FPath rayGenerationPath{ "" };
  FPath rayMissPath{ "" };
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


  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RAYTRACINGPIPELINE_H
