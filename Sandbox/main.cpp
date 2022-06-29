
#include <window/Window.h>
#include <window/WindowFactory.h>
#include <utilities/Logger.h>


using namespace uncanny;


auto main() -> i32 {
  FLogger::init(FLogger::sLoggerPtr);

  FWindowFactory windowFactory{};

  FWindowSpecification windowSpecification{};
  windowSpecification.name = "UncannyEngine v0.0.1";
  windowSpecification.width = 720;
  windowSpecification.height = 405;

  FWindow* pWindow{ windowFactory.createWindow<EWindowLibrary::GLFW>() };
  pWindow->init(windowSpecification);

  while(pWindow->isNotGoingToClose()) {
    pWindow->swapBuffersAndPollEvents();
  }

  pWindow->terminate();
  return 0;
}
