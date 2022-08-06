
#include <window/Window.h>
#include <window/WindowFactory.h>
#include <renderer/RenderContext.h>
#include <renderer/RenderContextFactory.h>
#include <utilities/Logger.h>


using namespace uncanny;


auto main() -> i32 {
  // Utilities setup
  FLogger::init(FLogger::sLoggerPtr);
  FRenderContextFactory renderContextFactory{};
  FWindowFactory windowFactory{};

  // Window subsystem configuration
  FWindowConfiguration windowConfiguration{};
  windowConfiguration.resizable = UFALSE;

  FWindowSpecification windowSpecification{};
  windowSpecification.name = "UncannySandbox";
  windowSpecification.width = 720;
  windowSpecification.height = 405;
  windowSpecification.config = windowConfiguration;

  FWindow* pWindow{ windowFactory.create<EWindowLibrary::GLFW>() };
  pWindow->init(windowSpecification);

  // Rendering subsystem configuration
  FRenderContextSpecification renderContextSpecification{};
  renderContextSpecification.pWindow = pWindow;
  renderContextSpecification.pAppName = "Uncanny Sandbox";
  renderContextSpecification.appVersion = UENGINE_MAKE_VERSION(0, 0, 1);
  renderContextSpecification.engineVersion = (u32)EEngineVersion::LATEST;

  FRenderContext* pRenderContext{ renderContextFactory.create<ERenderLibrary::VULKAN>() };
  pRenderContext->init(renderContextSpecification);

  // Running loop
  while(pWindow->isNotGoingToClose()) {
    pWindow->pollEvents();
  }

  // Terminating everything
  pRenderContext->terminate();
  pWindow->terminate();

  return 0;
}
