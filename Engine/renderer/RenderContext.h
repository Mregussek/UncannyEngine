
#ifndef UNCANNYENGINE_RENDERCONTEXT_H
#define UNCANNYENGINE_RENDERCONTEXT_H


#include <utilities/Includes.h>
#include "Mesh.h"


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


struct FRenderSceneConfiguration {
  FMesh* pMesh{};
};


class FRenderContext {
public:

  virtual b32 init(const FRenderContextSpecification& renderContextSpecs);
  virtual void terminate();

  virtual b32 parseSceneForRendering(const FRenderSceneConfiguration& sceneConfiguration);

  virtual b32 prepareStateForRendering();
  virtual ERenderContextState prepareFrame();
  virtual b32 submitFrame();
  virtual b32 endFrame();

};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
