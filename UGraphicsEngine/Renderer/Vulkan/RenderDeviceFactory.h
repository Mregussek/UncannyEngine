
#ifndef UNCANNYENGINE_RENDERDEVICEFACTORY_H
#define UNCANNYENGINE_RENDERDEVICEFACTORY_H


#include <volk.h>
#include "Resources/Buffer.h"


namespace uncanny::vulkan
{


class FPhysicalDevice;
class FLogicalDevice;


class FRenderDeviceFactory
{

  friend class FRenderDevice;

public:

  FBuffer CreateBuffer() const;

private:

  void Initialize(const FPhysicalDevice* pPhysicalDevice, const FLogicalDevice* pLogicalDevice);


  const FPhysicalDevice* m_pPhysicalDevice{ nullptr };
  const FLogicalDevice* m_pLogicalDevice{ nullptr };

};


}


#endif //UNCANNYENGINE_RENDERDEVICEFACTORY_H
