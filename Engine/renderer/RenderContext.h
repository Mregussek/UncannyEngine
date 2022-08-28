
#ifndef UNCANNYENGINE_RENDERCONTEXT_H
#define UNCANNYENGINE_RENDERCONTEXT_H


#include <utilities/Includes.h>


namespace uncanny
{


class FWindow;


enum class ERenderContextState {
  NONE, SURFACE_MINIMIZED, RENDERING
};


struct FRenderContextSpecification {
  const FWindow* pWindow{ nullptr };
  const char* pAppName{ nullptr };
  u32 appVersion{ 0 };
  u32 engineVersion{ 0 };
};


class FRenderContext {
public:

  virtual b32 init(FRenderContextSpecification renderContextSpecs);
  virtual void terminate();

  virtual b32 prepareStateForRendering();
  virtual ERenderContextState prepareFrame();
  virtual b32 submitFrame();
  virtual b32 endFrame();

};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
