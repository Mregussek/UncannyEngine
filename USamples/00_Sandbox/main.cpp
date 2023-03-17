
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
    while(not m_Window->IsGoingToClose()) {
      m_Window->UpdateState();
      m_Window->PollEvents();

      if (m_Window->IsMinimized())
      {
        continue;
      }

      m_RenderDevice.WaitForNextAvailableFrame();
      m_RenderDevice.SubmitSwapchainCommandBuffers();
      m_RenderDevice.PresentFrame();

      if (m_RenderDevice.IsOutOfDate())
      {
        m_RenderDevice.RecreateRenderingResources();
      }
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

    u32 backBufferCount{ 2 };
    m_RenderDevice.SetSwapchainCommandBuffersRecordingFunc([this]()
    {
      m_RenderDevice.RecordClearColorImageCommands();
    });
    m_RenderDevice.Create(m_Window, backBufferCount);
  }

  void Destroy() {
    m_RenderDevice.Destroy();
    m_Window->Destroy();
  }


  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderDevice m_RenderDevice{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
