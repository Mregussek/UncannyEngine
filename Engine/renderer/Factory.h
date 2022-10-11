
#ifndef UNCANNYENGINE_FACTORY_H
#define UNCANNYENGINE_FACTORY_H


#include "vulkan/vulkan_renderer/RendererVulkan.h"


namespace uncanny
{


class FRendererFactory {
public:

  template<ERenderLibrary TRenderLibrary>
  FRenderer* create() {
    if (TRenderLibrary == ERenderLibrary::VULKAN) {
      return &mRendererVulkan;
    }
    return nullptr;
  }

private:

  FRendererVulkan mRendererVulkan{};

};


}


#endif //UNCANNYENGINE_FACTORY_H
