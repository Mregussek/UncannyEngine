
#ifndef UNCANNYENGINE_RENDERCONTEXTVULKAN_H
#define UNCANNYENGINE_RENDERCONTEXTVULKAN_H


#include "renderer/RenderContext.h"
#include <volk.h>


namespace uncanny
{


class FRenderContextVulkan : public FRenderContext {
public:

  void init(FRenderContextSpecification renderContextSpecs) override;
  void terminate() override;

private:

  FRenderContextSpecification mSpecs;

  VkInstance m_instance{ VK_NULL_HANDLE };
  VkDebugReportCallbackEXT m_callback{ VK_NULL_HANDLE };
};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
