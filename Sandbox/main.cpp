
#include <window/Window.h>
#include <window/WindowFactory.h>
#include <renderer/RenderContext.h>
#include <renderer/RenderContextFactory.h>
#include <utilities/Logger.h>


using namespace uncanny;


auto main() -> i32 {
  FLogger::init(FLogger::sLoggerPtr);

  FRenderContextFactory renderContextFactory{};
  FWindowFactory windowFactory{};

  FWindowSpecification windowSpecification{};
  windowSpecification.name = "UncannySandbox";
  windowSpecification.width = 720;
  windowSpecification.height = 405;

  FWindow* pWindow{ windowFactory.create<EWindowLibrary::GLFW>() };
  pWindow->init(windowSpecification);

  FRenderContextSpecification renderContextSpecification;
  renderContextSpecification.pWindow = pWindow;
  renderContextSpecification.pAppName = "Uncanny Sandbox";
  renderContextSpecification.appVersion = UENGINE_MAKE_VERSION(0, 0, 1);
  renderContextSpecification.engineVersion = (u32)EEngineVersion::LATEST;

  FRenderContext* pRenderContext{ renderContextFactory.create<ERenderLibrary::VULKAN>() };
  pRenderContext->init(renderContextSpecification);

  while(pWindow->isNotGoingToClose()) {
    pWindow->swapBuffersAndPollEvents();
  }

  pRenderContext->terminate();
  pWindow->terminate();

  return 0;
}
