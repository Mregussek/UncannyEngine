
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderCommands.h"

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

      const vulkan::FSwapchain& swapchain = m_RenderDevice.GetSwapchain();

      VkSemaphore waitSemaphores[]{ swapchain.GetImageAvailableSemaphore().GetHandle() };
      VkSemaphore signalSemaphores[]{ swapchain.GetPresentableImageReadySemaphore().GetHandle() };
      VkFence fence{ swapchain.GetFence().GetHandle() };
      u32 frameIndex = swapchain.GetCurrentFrameIndex();
      vulkan::FCommandBuffer& commandBuffer = m_SwapchainCommandBuffers[frameIndex];

      m_RenderDevice.GetGraphicsQueue().Submit(waitSemaphores, commandBuffer, signalSemaphores, fence);
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
    m_RenderDevice.Create(m_Window, backBufferCount);
    m_RenderDevice.SetRecreateRenderingResourcesCallback([this](){ RecordCommands(); });

    backBufferCount = m_RenderDevice.GetSwapchain().GetBackBufferCount();
    m_SwapchainCommandBuffers = m_RenderDevice.GetGraphicsCommandPool().AllocatePrimaryCommandBuffers(backBufferCount);
    RecordCommands();
  }

  void Destroy() {
    m_RenderDevice.WaitIdle();

    std::ranges::for_each(m_SwapchainCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
    {
      commandBuffer.Free();
    });
    m_SwapchainCommandBuffers.clear();

    m_RenderDevice.Destroy();
    m_Window->Destroy();
  }

  void RecordCommands()
  {
    VkClearColorValue clearColorValue{ 1.0f, 0.8f, 0.4f, 0.0f };
    vulkan::FRenderCommands::RecordClearColorImage(m_SwapchainCommandBuffers,
                                                   m_RenderDevice.GetSwapchain().GetImages(),
                                                   clearColorValue);
  }


  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderDevice m_RenderDevice{};
  std::vector<vulkan::FCommandBuffer> m_SwapchainCommandBuffers{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
