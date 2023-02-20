
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include <UGraphicsEngine/RHI/RenderHardwareInterfaceVulkan.h>

using namespace uncanny;

int main() {
  FLog::create();
  FLog::trace("Hello UncannyEngine!");

  FWindowConfiguration windowConfiguration;
  windowConfiguration.fullscreen = UFALSE;
  windowConfiguration.resizable = UFALSE;
  windowConfiguration.width = 1600;
  windowConfiguration.height = 900;
  windowConfiguration.name = "UncannyEngine";

  FWindowGLFW window;
  window.Create(windowConfiguration);

  FRenderHardwareInterfaceVulkan rhi;
  rhi.Create();


  while(!window.IsGoingToClose()) {
    window.UpdateSize();
    window.PollEvents();
  }


  rhi.Destroy();

  window.Destroy();

  return 0;
}
