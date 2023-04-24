
#include "UTools/Logger/Log.h"
#include "UTools/Window/WindowGLFW.h"
#include "UTools/Filesystem/Path.h"
#include "UTools/Filesystem/File.h"
#include "UTools/Assets/AssetRegistry.h"
#include "UTools/Assets/MeshAsset.h"
#include "UTools/Assets/AssetLoader.h"
#include "UTools/EntityComponentSystem/EntityRegistry.h"
#include "UTools/EntityComponentSystem/Entity.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderContext.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/GlslShaderCompiler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Swapchain.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/RayTracingPipeline.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/AccelerationStructure.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Image.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Semaphore.h"
#include "UGraphicsEngine/Renderer/Camera.h"
#include "UGraphicsEngine/Renderer/RenderMesh.h"

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

      f32 deltaTime = m_Window->GetDeltaTime();

      m_Camera.ProcessKeyboardInput(m_Window.get(), deltaTime);
      m_Camera.ProcessMouseMovement(m_Window.get(), deltaTime);
      {
        FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
        m_CameraUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
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

        VkExtent2D swapchainExtent = m_Swapchain.GetCurrentExtent();
        m_OffscreenImage.Recreate(swapchainExtent);

        {
          u32 dstBinding = m_RayTracingDescriptorSetLayout.GetBindings()[1].binding;
          m_RayTracingDescriptorPool.WriteStorageImageToDescriptorSet(m_OffscreenImage.GetHandleView(), dstBinding);
        }

        m_Camera.UpdateAspectRatio((f32)swapchainExtent.width / (f32)swapchainExtent.height);
        {
          FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
          m_CameraUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
        }

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
            .width = 1600,
            .height = 900
        },
        .name = "UncannyEngine"
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
        .apiVersion = VK_API_VERSION_1_3
    };

    m_RenderContext.Create(renderContextAttributes, m_Window);
    const vulkan::FRenderDeviceFactory& deviceFactory = m_RenderContext.GetFactory();
    const vulkan::FLogicalDevice* pLogicalDevice = m_RenderContext.GetLogicalDevice();

    m_Swapchain.Create(2, pLogicalDevice->GetHandle(), &pLogicalDevice->GetPresentQueue(),
                       m_RenderContext.GetWindowSurface());
    u32 backBufferCount = m_Swapchain.GetBackBufferCount();
    VkExtent2D swapchainExtent = m_Swapchain.GetCurrentExtent();

    // Creating command pools
    m_CommandPool.Create(pLogicalDevice->GetGraphicsFamilyIndex(),
                         pLogicalDevice->GetHandle(),
                         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    // Creating command buffers...
    m_CommandBuffers = m_CommandPool.AllocatePrimaryCommandBuffers(backBufferCount);

    // Initializing ECS...
    m_EntityRegistry.Create();
    std::span<const FEntity> entities = m_EntityRegistry.Register(2);
    {
      FPath sponza = FPath::Append(FPath::GetEngineProjectPath(), {"resources", "sponza", "sponza.obj"});
      FMeshAsset& sponzaMeshAsset = m_AssetRegistry.RegisterMesh();
      sponzaMeshAsset.LoadObj(sponza.GetString().c_str());

      entities[0].Add<FRenderMeshComponent>(FRenderMeshComponent{
        .id = sponzaMeshAsset.ID(),
        .position = { 0.f, 2.f, 0.f },
        .rotation = { 0.f, 0.f, 0.f },
        .scale = { -1.f, -1.f, -1.f }
      });
    }
    {
      FPath bunny = FPath::Append(FPath::GetEngineProjectPath(), {"resources", "bunny", "bunny.obj"});
      FMeshAsset& bunnyMeshAsset = m_AssetRegistry.RegisterMesh();
      bunnyMeshAsset.LoadObj(bunny.GetString().c_str());

      entities[1].Add<FRenderMeshComponent>(FRenderMeshComponent{
          .id = bunnyMeshAsset.ID(),
          .position = { 0.f, 2.f, 0.f },
          .rotation = { 0.f, 90.f, 0.f },
          .scale = { -1.f, -1.f, -1.f }
      });
    }

    // Creating acceleration structures...
    std::vector<FRenderMesh> renderMeshes;
    renderMeshes.reserve(m_EntityRegistry.GetEntities().size());
    m_EntityRegistry.ForEach<FRenderMeshComponent>([this, &renderMeshes](FRenderMeshComponent& component)
    {
      const FMeshAsset& meshAsset = m_AssetRegistry.GetMesh(component.id);
      auto& renderMesh = renderMeshes.emplace_back(FRenderMeshFactory::ConvertAssetToOneRenderMesh(&meshAsset));
      renderMesh.transform = component.GetMatrix();
    });

    m_BottomLevelASVector = deviceFactory.CreateBottomLevelASVector(renderMeshes.size());
    for (u32 i = 0; i < renderMeshes.size(); i++)
    {
      m_BottomLevelASVector[i].Build(renderMeshes[i].vertices, renderMeshes[i].indices, renderMeshes[i].transform,
                                     m_CommandPool, pLogicalDevice->GetGraphicsQueue());
    }
    m_TopLevelAS = deviceFactory.CreateTopLevelAS();
    m_TopLevelAS.Build(m_BottomLevelASVector, m_CommandPool, pLogicalDevice->GetGraphicsQueue());

    // Creating camera...
    {
      FPerspectiveCameraSpecification cameraSpecification{
        .position = { -4.f, 0.f, 0.f },
        .front = { 0.f, 0.f, 0.f },
        .worldUp = { 0.f, 1.f, 0.f },
        .fieldOfView = 45.f,
        .aspectRatio = (f32)swapchainExtent.width / (f32)swapchainExtent.height,
        .near = 0.1f,
        .far = 10.f,
        .yaw = 0.f,
        .pitch = 0.f,
        .movementSpeed = 5.f,
        .sensitivity = 100.f,
        .zoom = 45.f
      };
      m_Camera.Initialize(cameraSpecification);
    }

    m_CameraUniformBuffer = deviceFactory.CreateBuffer();

    m_CameraUniformBuffer.Allocate(sizeof(FPerspectiveCameraUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    {
      FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
      m_CameraUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
    }

    // Creating off screen buffer...
    m_OffscreenImage = deviceFactory.CreateImage();
    {
      vulkan::FQueueFamilyIndex queueFamilies[]{ m_CommandPool.GetFamilyIndex() };
      m_OffscreenImage.Allocate(VK_FORMAT_B8G8R8A8_UNORM,
                                swapchainExtent,
                                VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                VK_IMAGE_LAYOUT_PREINITIALIZED,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                queueFamilies);
    }
    m_OffscreenImage.CreateView();

    // Creating objects buffers and descriptors
    m_ObjectsUniformBuffer = deviceFactory.CreateBuffer();
    {
      const auto& blasUniformData = m_TopLevelAS.GetBLASReferenceUniformData();
      m_ObjectsUniformBuffer.Allocate(blasUniformData.size() * sizeof(blasUniformData[0]),
                                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      m_ObjectsUniformBuffer.FillStaged(blasUniformData.data(), sizeof(blasUniformData[0]), blasUniformData.size(),
                                        m_CommandPool, pLogicalDevice->GetGraphicsQueue());
    }

    m_ObjectsDescriptorSetLayout = deviceFactory.CreateDescriptorSetLayout();
    m_ObjectsDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
      .pImmutableSamplers = nullptr
    });
    m_ObjectsDescriptorSetLayout.Create();

    m_ObjectsDescriptorPool = deviceFactory.CreateDescriptorPool();
    m_ObjectsDescriptorPool.Create(&m_ObjectsDescriptorSetLayout, 1);
    m_ObjectsDescriptorPool.AllocateDescriptorSet();
    {
      u32 dstBinding = m_ObjectsDescriptorSetLayout.GetBindings()[0].binding;
      VkDescriptorType type = m_ObjectsDescriptorSetLayout.GetBindings()[0].descriptorType;
      m_ObjectsDescriptorPool.WriteBufferToDescriptorSet(m_ObjectsUniformBuffer.GetHandle(),
                                                         m_ObjectsUniformBuffer.GetFilledStride(),
                                                         dstBinding, type);
    }

    // Creating ray tracing descriptors...
    m_RayTracingDescriptorSetLayout = deviceFactory.CreateDescriptorSetLayout();
    m_RayTracingDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
      .pImmutableSamplers = nullptr
    });
    m_RayTracingDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
        .pImmutableSamplers = nullptr
    });
    m_RayTracingDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
        .binding = 2,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
        .pImmutableSamplers = nullptr
    });
    m_RayTracingDescriptorSetLayout.Create();

    m_RayTracingDescriptorPool = deviceFactory.CreateDescriptorPool();
    m_RayTracingDescriptorPool.Create(&m_RayTracingDescriptorSetLayout, 1);
    m_RayTracingDescriptorPool.AllocateDescriptorSet();

    {
      u32 dstBinding = m_RayTracingDescriptorSetLayout.GetBindings()[0].binding;
      m_RayTracingDescriptorPool.WriteTopLevelAsToDescriptorSet(m_TopLevelAS.GetHandle(), dstBinding);
    }
    {
      u32 dstBinding = m_RayTracingDescriptorSetLayout.GetBindings()[1].binding;
      m_RayTracingDescriptorPool.WriteStorageImageToDescriptorSet(m_OffscreenImage.GetHandleView(), dstBinding);
    }
    {
      u32 dstBinding = m_RayTracingDescriptorSetLayout.GetBindings()[2].binding;
      VkDescriptorType type = m_RayTracingDescriptorSetLayout.GetBindings()[2].descriptorType;
      m_RayTracingDescriptorPool.WriteBufferToDescriptorSet(m_CameraUniformBuffer.GetHandle(),
                                                            m_CameraUniformBuffer.GetFilledStride(),
                                                            dstBinding, type);
    }

    // Creating ray tracing pipeline...
    m_RayTracingPipelineLayout = deviceFactory.CreatePipelineLayout();
    {
      VkDescriptorSetLayout setLayouts[]{ m_RayTracingDescriptorSetLayout.GetHandle(),
                                          m_ObjectsDescriptorSetLayout.GetHandle() };
      m_RayTracingPipelineLayout.Create(setLayouts);
    }

    FPath shadersPath = FPath::Append(FPath::GetEngineProjectPath(), { "UGraphicsEngine", "Renderer", "Vulkan",
                                                                       "Shaders" });
    m_RayTracingPipeline = deviceFactory.CreateRayTracingPipeline();
    vulkan::FGLSLShaderCompiler glslCompiler = deviceFactory.CreateGlslShaderCompiler();
    glslCompiler.Initialize();
    vulkan::FRayTracingPipelineSpecification rayTracingPipelineSpecification{
        .rayClosestHitPath = FPath::Append(shadersPath, "normals.rchit.spv"),
        .rayGenerationPath = FPath::Append(shadersPath, "camera.rgen"),
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
    for (vulkan::FBottomLevelAccelerationStructure& bottomAS : m_BottomLevelASVector)
    {
      bottomAS.Destroy();
    }
    m_TopLevelAS.Destroy();

    // Destroying descriptors...
    m_RayTracingDescriptorSetLayout.Destroy();
    m_RayTracingDescriptorPool.Destroy();

    m_ObjectsDescriptorSetLayout.Destroy();
    m_ObjectsDescriptorPool.Destroy();

    // Closing buffers
    m_CameraUniformBuffer.Free();
    m_ObjectsUniformBuffer.Free();

    // Destroying pipelines...
    m_RayTracingPipelineLayout.Destroy();
    m_RayTracingPipeline.Destroy();

    m_Swapchain.Destroy();
    m_RenderContext.Destroy();

    m_EntityRegistry.Destroy();

    m_AssetRegistry.Clear();

    m_Window->Destroy();
  }

  void RecordCommands()
  {
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
    VkDescriptorSet descriptorSets[]{ m_RayTracingDescriptorPool.GetDescriptorSet(),
                                      m_ObjectsDescriptorPool.GetDescriptorSet() };

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
      cmdBuf.BindDescriptorSets(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RayTracingPipelineLayout.GetHandle(),
                                descriptorSets);
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
  std::vector<vulkan::FBottomLevelAccelerationStructure> m_BottomLevelASVector{};
  vulkan::FTopLevelAccelerationStructure m_TopLevelAS{};
  vulkan::FDescriptorSetLayout m_RayTracingDescriptorSetLayout{};
  vulkan::FDescriptorPool m_RayTracingDescriptorPool{};
  vulkan::FPipelineLayout m_RayTracingPipelineLayout{};
  vulkan::FRayTracingPipeline m_RayTracingPipeline{};

  vulkan::FDescriptorSetLayout m_ObjectsDescriptorSetLayout{};
  vulkan::FDescriptorPool m_ObjectsDescriptorPool{};
  vulkan::FBuffer m_ObjectsUniformBuffer{};

  FPerspectiveCamera m_Camera{};
  vulkan::FBuffer m_CameraUniformBuffer{};

  FAssetRegistry m_AssetRegistry{};
  FEntityRegistry m_EntityRegistry{};

};


int main() {
  //return 0;
  Application app{};
  app.Run();

  return 0;
}
