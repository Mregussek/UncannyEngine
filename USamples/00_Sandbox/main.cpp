
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderCommands.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Image.h"

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

    m_RenderDevice.Create(m_Window, 2);
    m_RenderDevice.SetRecreateRenderingResourcesCallback([this]()
    {
      VkExtent2D currentExtent = m_RenderDevice.GetSwapchain().GetCurrentExtent();
      std::ranges::for_each(m_RenderTargetImages, [currentExtent](vulkan::FImage& image)
      {
        image.Recreate(currentExtent);
      });
      RecordCommands();
    });

    u32 backBufferCount = m_RenderDevice.GetSwapchain().GetBackBufferCount();

    // Creating buffers...
    m_Buffer = m_RenderDevice.GetFactory().CreateBuffer();
    m_Buffer.Allocate(16, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    // m_Buffer.Fill();

    // Creating render target images...
    m_RenderTargetImages = m_RenderDevice.GetFactory().CreateImages(backBufferCount);
    std::ranges::for_each(m_RenderTargetImages, [this](vulkan::FImage& image)
    {
      AllocateRenderTargetImage(image, m_RenderDevice.GetSwapchain().GetCurrentExtent());
    });

    // Creating swapchain command buffers...
    m_SwapchainCommandBuffers = m_RenderDevice.GetGraphicsCommandPool().AllocatePrimaryCommandBuffers(backBufferCount);
    RecordCommands();
  }

  void Destroy()
  {
    m_RenderDevice.WaitIdle();

    // Closing render target images...
    std::ranges::for_each(m_RenderTargetImages, [](vulkan::FImage& image)
    {
      image.Free();
    });

    // Closing swapchain command buffers...
    std::ranges::for_each(m_SwapchainCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
    {
      commandBuffer.Free();
    });

    m_Buffer.Free();

    m_RenderDevice.Destroy();
    m_Window->Destroy();
  }

  static void AllocateRenderTargetImage(vulkan::FImage& renderTargetImage, VkExtent2D extent)
  {
    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkImageUsageFlags usage =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    renderTargetImage.Allocate(format, extent, usage, initialLayout, memoryFlags);
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
  std::vector<vulkan::FImage> m_RenderTargetImages{};
  std::vector<vulkan::FCommandBuffer> m_SwapchainCommandBuffers{};
  vulkan::FBuffer m_Buffer{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
