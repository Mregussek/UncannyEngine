
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
  windowSpecification.name = "UncannyEngine v0.0.1";
  windowSpecification.width = 720;
  windowSpecification.height = 405;

  FWindow* pWindow{ windowFactory.create<EWindowLibrary::GLFW>() };
  pWindow->init(windowSpecification);

  FRenderContextSpecification renderContextSpecification;
  renderContextSpecification.pWindow = pWindow;

  FRenderContext* pRenderContext{ renderContextFactory.create<ERenderLibrary::VULKAN>() };
  pRenderContext->init(renderContextSpecification);

  while(pWindow->isNotGoingToClose()) {
    pWindow->swapBuffersAndPollEvents();
  }

  pRenderContext->terminate();
  pWindow->terminate();

  return 0;
}
