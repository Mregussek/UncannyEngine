
#ifndef UNCANNYENGINE_RENDERCONTEXT_H
#define UNCANNYENGINE_RENDERCONTEXT_H


#include <utilities/Includes.h>


namespace uncanny
{


class FWindow;


struct FRenderContextSpecification {

  FWindow* pWindow{ nullptr };

};


class FRenderContext {
public:

  virtual void init(FRenderContextSpecification renderContextSpecs);
  virtual void terminate();

};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
