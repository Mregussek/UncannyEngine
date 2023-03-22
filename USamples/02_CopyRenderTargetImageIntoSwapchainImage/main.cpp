
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderCommands.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Image.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Semaphore.h"

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
      u32 frameIndex = swapchain.GetCurrentFrameIndex();

      { // Submit work for rendering
        VkSemaphore signalSemaphores[]{ m_RenderSemaphores[frameIndex].GetHandle() };
        vulkan::FCommandBuffer& renderCommandBuffer = m_RenderCommandBuffers[frameIndex];
        m_RenderDevice.GetGraphicsQueue().Submit({}, {}, renderCommandBuffer, signalSemaphores, VK_NULL_HANDLE);
      }

      { // Submit work for copying render target into swapchain presentable image
        vulkan::FCommandBuffer& transferCommandBuffer = m_SwapchainTransferCommandBuffers[frameIndex];
        VkSemaphore waitSemaphores[]{ m_RenderSemaphores[frameIndex].GetHandle(),
                                      swapchain.GetImageAvailableSemaphore().GetHandle() };
        VkPipelineStageFlags waitStageFlags[]{ m_RenderCommandBuffers[frameIndex].GetLastWaitPipelineStage(),
                                               transferCommandBuffer.GetLastWaitPipelineStage() };
        VkSemaphore signalSemaphores[]{ swapchain.GetPresentableImageReadySemaphore().GetHandle() };
        VkFence fence{ swapchain.GetFence().GetHandle() };
        m_RenderDevice.GetTransferQueue().Submit(waitSemaphores, waitStageFlags, transferCommandBuffer,
                                                 signalSemaphores, fence);
      }

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

    // Creating render target images...
    m_RenderTargetImages = m_RenderDevice.GetFactory().CreateImages(backBufferCount);
    std::ranges::for_each(m_RenderTargetImages, [this](vulkan::FImage& image)
    {
      AllocateRenderTargetImage(image, m_RenderDevice.GetSwapchain().GetCurrentExtent());
    });

    // Creating synchronization objects...
    m_RenderSemaphores = m_RenderDevice.GetFactory().CreateSemaphores(backBufferCount);

    // Creating command buffers...
    m_RenderCommandBuffers = m_RenderDevice.GetGraphicsCommandPool().AllocatePrimaryCommandBuffers(backBufferCount);
    m_SwapchainTransferCommandBuffers = m_RenderDevice.GetTransferCommandPool().AllocatePrimaryCommandBuffers(backBufferCount);
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

    // Closing command buffers...
    std::ranges::for_each(m_RenderCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
    {
      commandBuffer.Free();
    });
    std::ranges::for_each(m_SwapchainTransferCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
    {
      commandBuffer.Free();
    });

    // Closing synchronization objects...
    std::ranges::for_each(m_RenderSemaphores, [](vulkan::FSemaphore& semaphore)
    {
      semaphore.Destroy();
    });

    m_Buffer.Free();

    m_RenderDevice.Destroy();
    m_Window->Destroy();
  }

  void AllocateRenderTargetImage(vulkan::FImage& renderTargetImage, VkExtent2D extent)
  {
    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkImageUsageFlags usage =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vulkan::FQueueFamilyIndex queueFamilies[]{ m_RenderDevice.GetGraphicsCommandPool().GetFamilyIndex(),
                                               m_RenderDevice.GetTransferCommandPool().GetFamilyIndex() };
    renderTargetImage.Allocate(format, extent, usage, initialLayout, memoryFlags, queueFamilies);
  }

  void RecordCommands()
  {
    std::vector<VkImage> vkRenderTargetImages(m_RenderTargetImages.size());
    std::transform(m_RenderTargetImages.begin(), m_RenderTargetImages.end(), vkRenderTargetImages.begin(),
                   [](vulkan::FImage& image) -> VkImage { return image.GetHandle(); });

    VkClearColorValue clearColorValue{ 1.0f, 0.8f, 0.4f, 0.0f };
    vulkan::FRenderCommands::RecordClearColorImage(m_RenderCommandBuffers, vkRenderTargetImages, clearColorValue,
                                                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    const vulkan::FSwapchain& swapchain = m_RenderDevice.GetSwapchain();
    vulkan::FRenderCommands::RecordCopyImage(m_SwapchainTransferCommandBuffers, vkRenderTargetImages,
                                             swapchain.GetImages(), swapchain.GetCurrentExtent(),
                                             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  }


  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderDevice m_RenderDevice{};
  std::vector<vulkan::FImage> m_RenderTargetImages{};
  std::vector<vulkan::FCommandBuffer> m_RenderCommandBuffers{};
  std::vector<vulkan::FSemaphore> m_RenderSemaphores{};
  std::vector<vulkan::FCommandBuffer> m_SwapchainTransferCommandBuffers{};
  vulkan::FBuffer m_Buffer{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
