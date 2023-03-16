
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderContextVulkan.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderDeviceVulkan.h"

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
      if (m_RenderContext.GetWindowSurface()->IsMinimized())
      {
        return;
      }
      m_RenderDevice.PrepareFrame();
      m_RenderDevice.RenderFrame();
      m_RenderDevice.PresentFrame();
      m_RenderDevice.EndFrame();

      m_Window->UpdateState();
      m_Window->PollEvents();
    }
  }

private:

  void Start() {
    FLog::create();

    FWindowConfiguration windowConfiguration{
      .resizable = UTRUE,
      .fullscreen = UFALSE,
      .size = {
          .width = 1600,
          .height = 900
      },
      .name = "UncannyEngine"
    };
    m_Window = std::make_shared<FWindowGLFW>();
    m_Window->Create(windowConfiguration);

    m_RenderContext.Create(m_Window);
    m_RenderDevice.Create(m_RenderContext.GetLogicalDevice(), m_RenderContext.GetWindowSurface());
  }

  void Destroy() {
    m_RenderDevice.Destroy();
    m_RenderContext.Destroy();
    m_Window->Destroy();
  }


  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderContext m_RenderContext{};
  vulkan::FRenderDevice m_RenderDevice{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
