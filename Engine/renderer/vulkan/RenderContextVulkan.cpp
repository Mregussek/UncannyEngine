
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FRenderContextVulkan::init(FRenderContextSpecification renderContextSpecs) {
  mSpecs = renderContextSpecs;
}


void FRenderContextVulkan::terminate() {
}


}
