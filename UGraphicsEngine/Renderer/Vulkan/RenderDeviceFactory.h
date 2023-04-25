
#ifndef UNCANNYENGINE_RENDERDEVICEFACTORY_H
#define UNCANNYENGINE_RENDERDEVICEFACTORY_H


#include <volk.h>
#include "Descriptors/DescriptorSetLayout.h"
#include "Descriptors/DescriptorPool.h"
#include "Resources/TopLevelAccelerationStructure.h"
#include "Resources/BottomLevelAccelerationStructure.h"
#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Device/GlslShaderCompiler.h"
#include "Device/PipelineLayout.h"
#include "Device/RayTracingPipeline.h"
#include "Device/RayTracingShadowPipeline.h"
#include "Synchronization/Semaphore.h"


namespace uncanny::vulkan
{


class FInstanceAttributes;
class FPhysicalDeviceAttributes;
class FLogicalDevice;


class FRenderDeviceFactory
{

  friend class FRenderContext;

public:

  [[nodiscard]] FBuffer CreateBuffer() const;

  [[nodiscard]] FBottomLevelAccelerationStructure CreateBottomLevelAS() const;

  [[nodiscard]] std::vector<FBottomLevelAccelerationStructure> CreateBottomLevelASVector(u32 count) const;

  [[nodiscard]] FTopLevelAccelerationStructure CreateTopLevelAS() const;

  [[nodiscard]] FImage CreateImage() const;

  [[nodiscard]] std::vector<FImage> CreateImages(u32 count) const;

  [[nodiscard]] FSemaphore CreateSemaphore() const;

  [[nodiscard]] std::vector<FSemaphore> CreateSemaphores(u32 count) const;

  [[nodiscard]] FGLSLShaderCompiler CreateGlslShaderCompiler() const;

  [[nodiscard]] FDescriptorSetLayout CreateDescriptorSetLayout() const;

  [[nodiscard]] FDescriptorPool CreateDescriptorPool() const;

  [[nodiscard]] FPipelineLayout CreatePipelineLayout() const;

  [[nodiscard]] FRayTracingPipeline CreateRayTracingPipeline() const;

  [[nodiscard]] FRayTracingShadowPipeline CreateRayTracingShadowPipeline() const;

private:

  void Initialize(const FInstanceAttributes* pInstanceAttributes,
                  const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, const FLogicalDevice* pLogicalDevice);


  const FInstanceAttributes* m_pInstanceAttributes{ nullptr };
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  const FLogicalDevice* m_pLogicalDevice{ nullptr };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEFACTORY_H
