
#include "RenderContext.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContext::init(FRenderContextSpecification renderContextSpecs) {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FRenderContext::prepareStateForRendering() {
  UFATAL("should not be used!");
  return UFALSE;
}


ERenderContextState FRenderContext::prepareFrame() {
  UFATAL("should not be used!");
  return ERenderContextState::NONE;
}


b32 FRenderContext::submitFrame() {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FRenderContext::endFrame() {
  UFATAL("should not be used!");
  return UFALSE;
}


void FRenderContext::terminate() {
  UFATAL("should not be used!");
}


}
