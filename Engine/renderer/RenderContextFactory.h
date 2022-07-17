
#ifndef UNCANNYENGINE_RENDERCONTEXTFACTORY_H
#define UNCANNYENGINE_RENDERCONTEXTFACTORY_H


#include "RenderContext.h"
#include "vulkan/RenderContextVulkan.h"


namespace uncanny
{


class FRenderContextFactory {
public:

  template<ERenderLibrary TRenderLibrary>
  FRenderContext* create() {
    if constexpr (TRenderLibrary == ERenderLibrary::VULKAN) {
      return &mRenderContextVulkan;
    }
  }

private:

  FRenderContextVulkan mRenderContextVulkan;

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTFACTORY_H
