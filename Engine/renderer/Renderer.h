
#ifndef UNCANNYENGINE_RENDERER_H
#define UNCANNYENGINE_RENDERER_H


#include <utilities/Includes.h>
#include "Mesh.h"


namespace uncanny
{


enum class ERendererState {
  NONE, SURFACE_MINIMIZED, RENDERING
};


struct FRenderSceneConfiguration {
  FMesh* pMesh{};
};


class FRenderer {
public:

  virtual b32 init();
  virtual void terminate();

  virtual b32 parseSceneForRendering(const FRenderSceneConfiguration& sceneConfiguration);
  virtual b32 closeScene();

  virtual b32 prepareStateForRendering();
  virtual ERendererState prepareFrame();
  virtual b32 submitFrame();
  virtual b32 endFrame();
};


}


#endif //UNCANNYENGINE_RENDERER_H
