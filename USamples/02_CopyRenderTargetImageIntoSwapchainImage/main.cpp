
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderContext.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Swapchain.h"
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

      m_Swapchain.WaitForNextImage();
      u32 frameIndex = m_Swapchain.GetCurrentFrameIndex();

      { // Submit work for rendering
        const vulkan::FQueue& graphicsQueue = m_RenderContext.GetLogicalDevice()->GetGraphicsQueue();
        vulkan::FCommandBuffer& renderCommandBuffer = m_RenderCommandBuffers[frameIndex];
        VkSemaphore signalSemaphores[]{ m_RenderSemaphores[frameIndex].GetHandle() };
        graphicsQueue.Submit({}, {}, renderCommandBuffer, signalSemaphores, VK_NULL_HANDLE);
      }

      { // Submit work for copying render target into swapchain presentable image
        const vulkan::FQueue& transferQueue = m_RenderContext.GetLogicalDevice()->GetTransferQueue();
        vulkan::FCommandBuffer& transferCommandBuffer = m_TransferCommandBuffers[frameIndex];
        VkSemaphore waitSemaphores[]{ m_RenderSemaphores[frameIndex].GetHandle(),
                                      m_Swapchain.GetImageAvailableSemaphore().GetHandle() };
        VkPipelineStageFlags waitStageFlags[]{ m_RenderCommandBuffers[frameIndex].GetLastWaitPipelineStage(),
                                               transferCommandBuffer.GetLastWaitPipelineStage() };
        VkSemaphore signalSemaphores[]{ m_Swapchain.GetPresentableImageReadySemaphore().GetHandle() };
        VkFence fence{ m_Swapchain.GetFence().GetHandle() };
        transferQueue.Submit(waitSemaphores, waitStageFlags, transferCommandBuffer, signalSemaphores, fence);
      }

      m_Swapchain.Present();

      if (m_Swapchain.IsOutOfDate() and m_Swapchain.IsRecreatePossible())
      {
        m_RenderContext.GetLogicalDevice()->WaitIdle();

        m_Swapchain.Recreate();

        m_GraphicsCommandPool.Reset();
        m_TransferCommandPool.Reset();

        VkExtent2D currentExtent = m_Swapchain.GetCurrentExtent();
        std::ranges::for_each(m_RenderTargetImages, [currentExtent](vulkan::FImage& image)
        {
          image.Recreate(currentExtent);
        });

        RecordRenderCommands();
        RecordTransferCommands();
      }
    }
  }

private:

  void Start() {
    FLog::create();

    // Creating window...
    FWindowConfiguration windowConfiguration{
        .resizable = UTRUE,
        .fullscreen = UFALSE,
        .size = {
            .width = 1600,
            .height = 900
        },
        .name = "UncannyEngine Sample 04 RayTracingTriangleWithCamera"
    };
    m_Window = std::make_shared<FWindowGLFW>();
    m_Window->Create(windowConfiguration);

    // Initialing renderer...
    vulkan::FRenderContextAttributes renderContextAttributes{
        .instanceLayers = { "VK_LAYER_KHRONOS_validation" },
        .instanceExtensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                               VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                               VK_EXT_DEBUG_UTILS_EXTENSION_NAME },
        .deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                              VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                              VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                              VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                              VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME },
        .pWindow = m_Window.get(),
        .apiVersion = VK_API_VERSION_1_3
    };
    m_RenderContext.Create(renderContextAttributes);

    const vulkan::FPhysicalDevice* pPhysicalDevice = m_RenderContext.GetPhysicalDevice();
    const vulkan::FLogicalDevice* pLogicalDevice = m_RenderContext.GetLogicalDevice();

    // Creating swapchain...
    m_Swapchain.Create(2, pLogicalDevice->GetHandle(), &pLogicalDevice->GetPresentQueue(),
                       m_RenderContext.GetWindowSurface());

    // Creating command pools
    m_GraphicsCommandPool.Create(m_RenderContext.GetLogicalDevice()->GetGraphicsFamilyIndex(),
                                 m_RenderContext.GetLogicalDevice()->GetHandle(),
                                 0);
    m_TransferCommandPool.Create(m_RenderContext.GetLogicalDevice()->GetTransferFamilyIndex(),
                                 m_RenderContext.GetLogicalDevice()->GetHandle(),
                                 0);

    // Creating all back buffer dependent resources
    u32 backBufferCount = m_Swapchain.GetBackBufferCount();
    m_RenderTargetImages.reserve(backBufferCount);
    m_RenderSemaphores.reserve(backBufferCount);
    for (u32 i = 0; i < backBufferCount; i++)
    {
      { // render target images...
        vulkan::FImage& image = m_RenderTargetImages.emplace_back(pLogicalDevice->GetHandle(),
                                                                  &pPhysicalDevice->GetAttributes());
        VkExtent2D extent = m_Swapchain.GetCurrentExtent();
        VkFormat format = m_Swapchain.GetFormat();
        VkImageUsageFlags usage =
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        vulkan::FQueueFamilyIndex queueFamilies[]{ m_GraphicsCommandPool.GetFamilyIndex(),
                                                   m_TransferCommandPool.GetFamilyIndex() };
        image.Allocate(format, extent, usage, initialLayout, memoryFlags, queueFamilies);
      }
      { // semaphores...
        vulkan::FSemaphore& semaphore = m_RenderSemaphores.emplace_back();
        semaphore.Create(pLogicalDevice->GetHandle());
      }
    }

    // Creating command buffers...
    m_RenderCommandBuffers = m_GraphicsCommandPool.AllocatePrimaryCommandBuffers(backBufferCount);
    m_TransferCommandBuffers = m_TransferCommandPool.AllocatePrimaryCommandBuffers(backBufferCount);

    RecordRenderCommands();
    RecordTransferCommands();
  }

  void Destroy()
  {
    if (m_RenderContext.GetLogicalDevice()->IsValid())
    {
      m_RenderContext.GetLogicalDevice()->WaitIdle();
    }

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
    std::ranges::for_each(m_TransferCommandBuffers, [](vulkan::FCommandBuffer& commandBuffer)
    {
      commandBuffer.Free();
    });

    // Closing Command Pools...
    m_GraphicsCommandPool.Destroy();
    m_TransferCommandPool.Destroy();

    // Closing synchronization objects...
    std::ranges::for_each(m_RenderSemaphores, [](vulkan::FSemaphore& semaphore)
    {
      semaphore.Destroy();
    });

    m_Swapchain.Destroy();
    m_RenderContext.Destroy();
    m_Window->Destroy();
  }

  void RecordRenderCommands()
  {
    VkClearColorValue clearColorValue{ 1.0f, 0.8f, 0.4f, 0.0f };
    for (u32 i = 0; i < m_RenderCommandBuffers.size(); i++)
    {
      VkImage image = m_RenderTargetImages[i].GetHandle();
      vulkan::FCommandBuffer& renderCmdBuf = m_RenderCommandBuffers[i];

      VkImageSubresourceRange subresourceRange{
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1
      };

      renderCmdBuf.BeginRecording();
      renderCmdBuf.ImageMemoryBarrier(image,
                                      VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                      VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      subresourceRange,
                                      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
      renderCmdBuf.ClearColorImage(image, clearColorValue, subresourceRange);
      renderCmdBuf.ImageMemoryBarrier(image,
                                      VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                      subresourceRange,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
      renderCmdBuf.EndRecording();
    }
  }

  void RecordTransferCommands()
  {
    for (u32 i = 0; i < m_TransferCommandBuffers.size(); i++)
    {
      vulkan::FCommandBuffer& transferCmdBuf = m_TransferCommandBuffers[i];
      VkImage srcImage = m_RenderTargetImages[i].GetHandle();
      VkImage dstImage = m_Swapchain.GetImages()[i];
      VkExtent2D extent = m_Swapchain.GetCurrentExtent();

      VkImageSubresourceRange subresourceRange{
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1
      };
      VkImageSubresourceLayers subresourceLayers{
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .mipLevel = 0,
          .baseArrayLayer = 0,
          .layerCount = 1
      };

      transferCmdBuf.BeginRecording();
      transferCmdBuf.ImageMemoryBarrier(dstImage,
                                        VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        subresourceRange,
                                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
      transferCmdBuf.CopyImage(srcImage, dstImage, subresourceLayers, extent);
      transferCmdBuf.ImageMemoryBarrier(dstImage,
                                        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                        subresourceRange,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
      transferCmdBuf.EndRecording();
    }
  }


  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderContext m_RenderContext{};
  vulkan::FSwapchain m_Swapchain{};
  vulkan::FCommandPool m_GraphicsCommandPool{};
  vulkan::FCommandPool m_TransferCommandPool{};
  std::vector<vulkan::FCommandBuffer> m_RenderCommandBuffers{};
  std::vector<vulkan::FCommandBuffer> m_TransferCommandBuffers{};
  std::vector<vulkan::FImage> m_RenderTargetImages{};
  std::vector<vulkan::FSemaphore> m_RenderSemaphores{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
