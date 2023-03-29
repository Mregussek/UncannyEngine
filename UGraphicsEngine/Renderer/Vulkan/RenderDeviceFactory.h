
#ifndef UNCANNYENGINE_RENDERDEVICEFACTORY_H
#define UNCANNYENGINE_RENDERDEVICEFACTORY_H


#include <volk.h>
#include "UGraphicsEngine/Renderer/Vulkan/Resources/BottomLevelAS.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/TopLevelAS.h"
#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Synchronization/Semaphore.h"


namespace uncanny::vulkan
{


class FPhysicalDevice;
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

private:

  void Initialize(const FPhysicalDevice* pPhysicalDevice, const FLogicalDevice* pLogicalDevice);


  const FPhysicalDevice* m_pPhysicalDevice{ nullptr };
  const FLogicalDevice* m_pLogicalDevice{ nullptr };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEFACTORY_H
