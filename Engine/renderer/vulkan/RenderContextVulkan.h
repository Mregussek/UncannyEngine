
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

  b32 createInstance();
  b32 closeInstance();


  FRenderContextSpecification mSpecs{};

  VkInstance mInstanceVk{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
