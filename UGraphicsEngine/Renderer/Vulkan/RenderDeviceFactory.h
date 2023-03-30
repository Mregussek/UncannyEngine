
#ifndef UNCANNYENGINE_RENDERDEVICEFACTORY_H
#define UNCANNYENGINE_RENDERDEVICEFACTORY_H


#include <volk.h>
#include "Descriptors/DescriptorSetLayout.h"
#include "Descriptors/DescriptorPool.h"
#include "Resources/BottomLevelAS.h"
#include "Resources/TopLevelAS.h"
#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Device/GlslShaderCompiler.h"
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

  [[nodiscard]] FBottomLevelAS CreateBottomLevelAS() const;

  [[nodiscard]] FTopLevelAS CreateTopLevelAS() const;

  [[nodiscard]] std::vector<FImage> CreateImages(u32 count) const;

  [[nodiscard]] std::vector<FSemaphore> CreateSemaphores(u32 count) const;

  [[nodiscard]] FGLSLShaderCompiler CreateGlslShaderCompiler() const;

  [[nodiscard]] FDescriptorSetLayout CreateDescriptorSetLayout() const;

  [[nodiscard]] FDescriptorPool CreateDescriptorPool() const;

private:

  void Initialize(const FInstanceAttributes* pInstanceAttributes,
                  const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, const FLogicalDevice* pLogicalDevice);


  const FInstanceAttributes* m_pInstanceAttributes{ nullptr };
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  const FLogicalDevice* m_pLogicalDevice{ nullptr };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEFACTORY_H
