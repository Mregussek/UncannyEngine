
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderHardwareInterfaceVulkan.h"

using namespace uncanny;


class Application {
public:

  Application() {
    Start();
  }

  ~Application() {
    Destroy();
  }

  void Run() {
    while(!m_Window->IsGoingToClose()) {
      m_Window->UpdateSize();
      m_Window->PollEvents();
    }
  }

private:

  void Start() {
    FLog::create();

    FWindowConfiguration windowConfiguration;
    windowConfiguration.fullscreen = UFALSE;
    windowConfiguration.resizable = UFALSE;
    windowConfiguration.width = 1600;
    windowConfiguration.height = 900;
    windowConfiguration.name = "UncannyEngine";

    m_Window = std::make_shared<FWindowGLFW>();
    m_Window->Create(windowConfiguration);

    m_RHI.Create(m_Window);
  }

  void Destroy() {
    m_RHI.Destroy();
    m_Window->Destroy();
  }


  std::shared_ptr<IWindow> m_Window;
  FRenderHardwareInterfaceVulkan m_RHI{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}