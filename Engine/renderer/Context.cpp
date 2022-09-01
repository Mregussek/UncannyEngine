
#include "Context.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContext::init(const FRenderContextSpecification& renderContextSpecs) {
  UFATAL("should not be used!");
  return UFALSE;
}


void FRenderContext::terminate() {
  UFATAL("should not be used!");
}


ERenderLibrary FRenderContext::getLibrary() const {
  UFATAL("should not be used!");
  return ERenderLibrary::NONE;
}


}
