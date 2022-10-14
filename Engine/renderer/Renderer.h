
#ifndef UNCANNYENGINE_RENDERER_H
#define UNCANNYENGINE_RENDERER_H


#include <utilities/Includes.h>
#include "Mesh.h"


namespace uncanny
{


class FCamera;
class FWindow;


enum class ERendererState {
  NONE, SURFACE_MINIMIZED, RENDERING
};


struct FRenderSceneConfiguration {
  FMesh* pMesh{ nullptr };
  FCamera* pCamera{ nullptr };
};


struct FRendererSpecification {
  const FWindow* pWindow{ nullptr };
  const char* appName{ "" };
  u32 engineVersion{ UVERSION_UNDEFINED };
  u32 appVersion{ UVERSION_UNDEFINED };
};


struct FRendererPrepareFrameSpecification {
};


struct FRendererEndFrameSpecification {
};


class FRenderer {
public:

  virtual b32 init(const FRendererSpecification& specs);
  virtual void terminate();

  virtual b32 parseSceneForRendering(const FRenderSceneConfiguration& sceneConfig);
  virtual b32 updateSceneDuringRendering(const FRenderSceneConfiguration& sceneConfig);
  virtual b32 closeScene();

  virtual b32 prepareStateForRendering();
  virtual ERendererState prepareFrame(const FRendererPrepareFrameSpecification& specs);
  virtual b32 submitFrame();
  virtual b32 endFrame(const FRendererEndFrameSpecification& specs);

};


}


#endif //UNCANNYENGINE_RENDERER_H
