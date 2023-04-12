
#include <UTools/Logger/Log.h>
#include <UTools/Window/WindowGLFW.h>
#include <UTools/Filesystem/Path.h>
#include <UTools/Filesystem/File.h>
#include "UGraphicsEngine/Renderer/Vulkan/RenderContext.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/GlslShaderCompiler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Swapchain.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Mesh.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/RayTracingPipeline.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/BottomLevelAS.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/TopLevelAS.h"
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

      const vulkan::FQueue& graphicsQueue = m_RenderContext.GetLogicalDevice()->GetGraphicsQueue();
      VkSemaphore waitSemaphores[]{ m_Swapchain.GetImageAvailableSemaphore().GetHandle() };
      VkPipelineStageFlags waitStageFlags[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
      VkSemaphore signalSemaphores[]{ m_Swapchain.GetPresentableImageReadySemaphore().GetHandle() };
      VkFence fence{ m_Swapchain.GetFence().GetHandle() };
      graphicsQueue.Submit(waitSemaphores, waitStageFlags, m_CommandBuffers[frameIndex], signalSemaphores, fence);

      m_Swapchain.Present();

      if (m_Swapchain.IsOutOfDate() and m_Swapchain.IsRecreatePossible())
      {
        m_RenderContext.GetLogicalDevice()->WaitIdle();

        m_Swapchain.Recreate();

        m_CommandPool.Reset();

        m_OffscreenImage.Recreate(m_Swapchain.GetCurrentExtent());

        RecordCommands();
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
          .width = 640,
          .height = 480
      },
      .name = "UncannyEngine"
    };
    m_Window = std::make_shared<FWindowGLFW>();
    m_Window->Create(windowConfiguration);

    vulkan::FRenderContextAttributes renderContextAttributes{
      .instanceLayers = { "VK_LAYER_KHRONOS_validation" },
      .instanceExtensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                             VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                             VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                             VK_EXT_DEBUG_UTILS_EXTENSION_NAME },
      .deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                            VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                            VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                            VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                            VK_KHR_SPIRV_1_4_EXTENSION_NAME,
                            VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME },
      .apiVersion = VK_API_VERSION_1_3
    };

    m_RenderContext.Create(renderContextAttributes, m_Window);
    const vulkan::FRenderDeviceFactory& deviceFactory = m_RenderContext.GetFactory();
    const vulkan::FLogicalDevice* pLogicalDevice = m_RenderContext.GetLogicalDevice();

    m_Swapchain.Create(2, pLogicalDevice->GetHandle(), &pLogicalDevice->GetPresentQueue(),
                       m_RenderContext.GetWindowSurface());
    u32 backBufferCount = m_Swapchain.GetBackBufferCount();

    // Creating command pools
    m_CommandPool.Create(pLogicalDevice->GetGraphicsFamilyIndex(),
                         pLogicalDevice->GetHandle(),
                         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    // Creating command buffers...
    m_CommandBuffers = m_CommandPool.AllocatePrimaryCommandBuffers(backBufferCount);

    // Creating buffers and acceleration structures...
    std::vector<vulkan::FVertex> vertices{
      { .position = { .x = 1.f, .y = 1.f, .z = 0.f } },
      { .position = { .x = -1.f, .y = 1.f, .z = 0.f } },
      { .position = { .x = 0.f, .y = -1.f, .z = 0.f } },
    };
    std::vector<u32> indices{ 0, 1, 2 };

    m_BottomLevelAS = deviceFactory.CreateBottomLevelAS();
    m_BottomLevelAS.Build(vertices, indices, m_CommandPool, pLogicalDevice->GetGraphicsQueue());

    m_TopLevelAS = deviceFactory.CreateTopLevelAS();
    m_TopLevelAS.Build(m_BottomLevelAS, m_CommandPool, pLogicalDevice->GetGraphicsQueue());

    // Creating render target images...
    m_OffscreenImage = deviceFactory.CreateImage();
    {
      VkExtent2D extent = m_Swapchain.GetCurrentExtent();
      VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
      VkImageUsageFlags usage =
          VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
      VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      vulkan::FQueueFamilyIndex queueFamilies[]{ m_CommandPool.GetFamilyIndex() };
      m_OffscreenImage.Allocate(format, extent, usage, initialLayout, memoryFlags, queueFamilies);
    }
    m_OffscreenImage.CreateView();

    // Creating descriptors...
    m_DescriptorSetLayout = deviceFactory.CreateDescriptorSetLayout();
    {
      u32 binding = 0;
      VkDescriptorType type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
      u32 count = 1;
      VkShaderStageFlags stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
      m_DescriptorSetLayout.AddBinding(binding, type, count, stageFlags, nullptr);
    }
    {
      u32 binding = 1;
      VkDescriptorType type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      u32 count = 1;
      VkShaderStageFlags stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
      m_DescriptorSetLayout.AddBinding(binding, type, count, stageFlags, nullptr);
    }
    m_DescriptorSetLayout.Create();

    m_DescriptorPool = deviceFactory.CreateDescriptorPool();
    m_DescriptorPool.Create(&m_DescriptorSetLayout, 1);
    m_DescriptorPool.AllocateDescriptorSets(1);

    {
      u32 dstBinding = m_DescriptorSetLayout.GetBindings()[0].binding;
      m_DescriptorPool.WriteTopLevelAsToDescriptorSets(m_TopLevelAS.GetHandle(), dstBinding);
    }
    {
      u32 dstBinding = m_DescriptorSetLayout.GetBindings()[1].binding;
      m_DescriptorPool.WriteStorageImageToDescriptorSets(m_OffscreenImage, dstBinding);
    }

    // Creating pipeline...
    m_RayTracingPipelineLayout = deviceFactory.CreatePipelineLayout();
    VkDescriptorSetLayout setLayouts[]{ m_DescriptorSetLayout.GetHandle() };
    m_RayTracingPipelineLayout.Create(setLayouts);

    FPath shadersPath = FPath::Append(FPath::GetEngineProjectPath(), { "UGraphicsEngine", "Renderer", "Vulkan",
                                                                       "Shaders" });
    m_RayTracingPipeline = deviceFactory.CreateRayTracingPipeline();
    vulkan::FGLSLShaderCompiler glslCompiler = deviceFactory.CreateGlslShaderCompiler();
    glslCompiler.Initialize();
    vulkan::FRayTracingPipelineSpecification rayTracingPipelineSpecification{
      .rayClosestHitPath = FPath::Append(shadersPath, "default.rchit"),
      .rayGenerationPath = FPath::Append(shadersPath, "default.rgen"),
      .rayMissPath =  FPath::Append(shadersPath, "default.rmiss"),
      .pGlslCompiler = &glslCompiler,
      .pPipelineLayout = &m_RayTracingPipelineLayout
    };
    m_RayTracingPipeline.Create(rayTracingPipelineSpecification);

    // Recording commands
    RecordCommands();
  }

  void Destroy()
  {
    if (m_RenderContext.GetLogicalDevice()->IsValid())
    {
      m_RenderContext.GetLogicalDevice()->WaitIdle();
    }

    // Closing render target images...
    m_OffscreenImage.Free();

    // Closing command buffers...
    std::ranges::for_each(m_CommandBuffers, [](vulkan::FCommandBuffer& cmdBuf)
    {
      cmdBuf.Free();
    });

    // Closing Command Pools...
    m_CommandPool.Destroy();

    // Destroying rendering resources...
    m_BottomLevelAS.Destroy();
    m_TopLevelAS.Destroy();

    // Destroying descriptors...
    m_DescriptorSetLayout.Destroy();
    m_DescriptorPool.Destroy();

    // Destroying pipelines...
    m_RayTracingPipelineLayout.Destroy();
    m_RayTracingPipeline.Destroy();

    m_Swapchain.Destroy();
    m_RenderContext.Destroy();
    m_Window->Destroy();
  }

  void RecordCommands()
  {
    VkClearColorValue clearColorValue{ 1.0f, 0.8f, 0.4f, 0.0f };
    VkExtent3D offscreenExtent = m_OffscreenImage.GetExtent3D();
    VkImage offscreenImage = m_OffscreenImage.GetHandle();

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

    std::span<const VkImage> swapchainImages = m_Swapchain.GetImages();
    VkExtent2D swapchainExtent = m_Swapchain.GetCurrentExtent();
    for (u32 i = 0; i < swapchainImages.size(); i++)
    {
      vulkan::FCommandBuffer& cmdBuf = m_CommandBuffers[i];
      VkImage swapchainImage = swapchainImages[i];

      cmdBuf.BeginRecording();
      cmdBuf.ImageMemoryBarrier(offscreenImage,
                                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                                subresourceRange,
                                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
      cmdBuf.BindPipeline(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RayTracingPipeline.GetHandle());
      cmdBuf.BindDescriptorSet(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RayTracingPipelineLayout.GetHandle(),
                               m_DescriptorPool.GetDescriptorSet(0));
      cmdBuf.TraceRays(&m_RayTracingPipeline, offscreenExtent);
      cmdBuf.ImageMemoryBarrier(offscreenImage,
                                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                subresourceRange,
                                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
      cmdBuf.ImageMemoryBarrier(swapchainImage,
                                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                subresourceRange,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
      cmdBuf.CopyImage(offscreenImage, swapchainImage, subresourceLayers, swapchainExtent);
      cmdBuf.ImageMemoryBarrier(swapchainImage,
                                VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                subresourceRange,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
      cmdBuf.EndRecording();
    }
  }


  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderContext m_RenderContext{};
  vulkan::FSwapchain m_Swapchain{};
  vulkan::FCommandPool m_CommandPool{};
  std::vector<vulkan::FCommandBuffer> m_CommandBuffers{};
  vulkan::FImage m_OffscreenImage{};
  vulkan::FSemaphore m_PresentAvailableSemaphore{};
  vulkan::FBottomLevelAS m_BottomLevelAS{};
  vulkan::FTopLevelAS m_TopLevelAS{};
  vulkan::FDescriptorSetLayout m_DescriptorSetLayout{};
  vulkan::FDescriptorPool m_DescriptorPool{};
  vulkan::FPipelineLayout m_RayTracingPipelineLayout{};
  vulkan::FRayTracingPipeline m_RayTracingPipeline{};

};


int main() {
  Application app{};
  app.Run();

  return 0;
}
