
#ifndef UNCANNYENGINE_FACTORY_H
#define UNCANNYENGINE_FACTORY_H


#include "Context.h"
#include "vulkan_context/ContextVulkan.h"
#include "vulkan_renderer/RendererVulkan.h"


namespace uncanny
{


class FRenderContextFactory {
public:

  template<ERenderLibrary TRenderLibrary>
  FRenderContext* create() {
    if constexpr (TRenderLibrary == ERenderLibrary::VULKAN) {
      return &mContextVulkan;
    }
    return nullptr;
  }

  FRenderer* retrieveRendererFrom(FRenderContext* pContext) {
    if (pContext->getLibrary() == ERenderLibrary::VULKAN) {
      mRendererVulkan.passContext(&mContextVulkan);
      return &mRendererVulkan;
    }
    return nullptr;
  }

private:

  FRenderContextVulkan mContextVulkan{};
  FRendererVulkan mRendererVulkan{};

};


}


#endif //UNCANNYENGINE_FACTORY_H
