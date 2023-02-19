
#include <UTools/Logger/Log.h>
#include "UGraphicsEngine/RHI/RenderHardwareInterfaceVulkan.h"

using namespace uncanny;

int main() {
  FLogger::create();
  FLogger::trace("Hello UncannyEngine!");

  FRenderHardwareInterfaceVulkan rhi;
  rhi.Create();


  rhi.Destroy();

  return 0;
}
