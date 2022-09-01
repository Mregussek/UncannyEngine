
#ifndef UNCANNYENGINE_CONTEXT_H
#define UNCANNYENGINE_CONTEXT_H


#include <utilities/Includes.h>
#include <utilities/Libraries.h>


namespace uncanny
{


class FWindow;


struct FRenderContextSpecification {
  const FWindow* pWindow{ nullptr };
  const char* pAppName{ nullptr };
  u32 appVersion{ 0 };
  u32 engineVersion{ 0 };
};


class FRenderContext {
public:

  virtual b32 init(const FRenderContextSpecification& renderContextSpecs);
  virtual void terminate();

  virtual ERenderLibrary getLibrary() const;

};


}


#endif //UNCANNYENGINE_CONTEXT_H
