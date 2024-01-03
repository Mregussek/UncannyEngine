
#include "UTools/Logger/Log.h"
#include "UMath/Trig.h"
#include "UTools/Window/WindowGLFW.h"
#include "UTools/Filesystem/Path.h"
#include "UTools/Filesystem/File.h"
#include "UTools/Assets/AssetRegistry.h"
#include "UTools/Assets/MeshAsset.h"
#include "UTools/EntityComponentSystem/EntityRegistryLoader.h"
#include "UTools/EntityComponentSystem/EntityRegistry.h"
#include "UTools/EntityComponentSystem/Entity.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderContext.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/GlslShaderCompiler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Swapchain.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/RayTracingPipeline.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Image.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/BottomLevelAccelerationStructure.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/TopLevelAccelerationStructure.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Semaphore.h"
#include "UGraphicsEngine/Renderer/PerspectiveCamera.h"
#include "UGraphicsEngine/Renderer/RenderMesh.h"
#include "UGraphicsEngine/Renderer/Light.h"

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

      m_Camera.ProcessMovement(m_Window.get(), deltaTime);
      {
        FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
        m_PerFrameUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
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

        m_Camera.SetAspectRatio((f32)swapchainExtent.width / (f32)swapchainExtent.height);
        {
          FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
          m_PerFrameUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
        }

        RecordCommands();
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
        .name = "UncannyEngine Sample 11 ReflectionsAlongWithSceneLoadingFromJson"
    };
    m_Window = std::make_shared<FWindowGLFW>();
    m_Window->Create(windowConfiguration);

    // Initialing renderer...
    vulkan::FRenderContextAttributes renderContextAttributes{
      .instanceLayers = { "VK_LAYER_KHRONOS_validation" },
      .instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME,
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

    // Creating command pools...
    m_CommandPool.Create(pLogicalDevice->GetGraphicsFamilyIndex(),
                         pLogicalDevice->GetHandle(),
                         VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    // Creating command buffers...
    m_CommandBuffers = m_CommandPool.AllocatePrimaryCommandBuffers(m_Swapchain.GetBackBufferCount());

    // Registering entities with render mesh components and loading several obj files...
    m_EntityRegistry.Create();
    FPath sceneJsonPath = FPath::Append(FPath::GetEngineProjectPath(), { "USceneSamples", "3_Sponza_Additions.json" });
    FEntityRegistryLoader::LoadJsonScene(sceneJsonPath.GetStringPath().c_str(), &m_EntityRegistry, &m_AssetRegistry);

    // Converting asset meshes and materials into render meshes and materials...
    std::vector<FRenderData> renderDataVector;
    renderDataVector.reserve(m_EntityRegistry.GetEntities().size());
    m_EntityRegistry.ForEach<FRenderMeshComponent>([this, &renderDataVector](FRenderMeshComponent& component)
    {
      const FMeshAsset& meshAsset = m_AssetRegistry.GetMesh(component.id);
      renderDataVector.emplace_back(FRenderMeshFactory::ConvertAssetToOneRenderData(&meshAsset, component.GetMatrix()));
    });

    // Creating acceleration structures...
    m_BottomLevelAccelerationVector.reserve(renderDataVector.size());
    for (auto& data : renderDataVector)
    {
      auto& bottomAS = m_BottomLevelAccelerationVector.emplace_back();
      bottomAS.Build(data.mesh, data.materials, m_CommandPool, pLogicalDevice->GetGraphicsQueue(),
                     pLogicalDevice->GetHandle(), &pPhysicalDevice->GetAttributes());
    }
    m_TopLevelAS.Build(m_BottomLevelAccelerationVector, m_CommandPool, pLogicalDevice->GetGraphicsQueue(),
                       pLogicalDevice->GetHandle(),&pPhysicalDevice->GetAttributes());

    // Creating blas reference uniform buffer...
    {
      const auto& blasUniformData = m_TopLevelAS.GetBLASReferenceUniformData();
      m_BLASReferenceUniformBuffer.Allocate(blasUniformData.size() * sizeof(blasUniformData[0]),
                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pLogicalDevice->GetHandle(),
                                            &pPhysicalDevice->GetAttributes());
      m_BLASReferenceUniformBuffer.FillStaged(blasUniformData.data(), sizeof(blasUniformData[0]),
                                              blasUniformData.size(), m_CommandPool,
                                              pLogicalDevice->GetGraphicsQueue());
    }

    // Creating camera...
    {
      VkExtent2D swapchainExtent = m_Swapchain.GetCurrentExtent();
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
        .zoom = 45.f,
        .constrainPitch = UTRUE
      };
      m_Camera.Initialize(cameraSpecification);

      FCameraRayTracingSpecification rayTracingSpecification{
        .maxFrameCounterLimit = 4096,
        .maxRayBounces = 8,
        .maxSamplesPerPixel = 4
      };
      m_Camera.SetRayTracingSpecification(rayTracingSpecification);
    }

    m_PerFrameUniformBuffer.Allocate(sizeof(FPerspectiveCameraUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, pLogicalDevice->GetHandle(),
                                   &pPhysicalDevice->GetAttributes());
    {
      FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
      m_PerFrameUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
    }

    // Creating off screen buffer...
    {
      VkFormat swapchainFormat = m_Swapchain.GetFormat();
      VkExtent2D swapchainExtent = m_Swapchain.GetCurrentExtent();
      vulkan::FQueueFamilyIndex queueFamilies[]{ m_CommandPool.GetFamilyIndex() };
      VkImageUsageFlags flags =
          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      m_OffscreenImage.Allocate(swapchainFormat, swapchainExtent, flags, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, queueFamilies, pLogicalDevice->GetHandle(),
                                &pPhysicalDevice->GetAttributes());
    }
    m_OffscreenImage.CreateView();

    // Creating light buffer
    m_Light.position = { -1.f, -1.f, 0.2f };
    {
      FLightUniformData uniformData{ .position = m_Light.position };
      m_LightUniformBuffer.Allocate(sizeof(FLightUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    pLogicalDevice->GetHandle(),&pPhysicalDevice->GetAttributes());
      m_LightUniformBuffer.Fill(&uniformData, sizeof(FLightUniformData), 1);
    }

    // Creating scene descriptors...
    m_SceneDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
      .pImmutableSamplers = nullptr
    });
    m_SceneDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
        .pImmutableSamplers = nullptr
    });
    m_SceneDescriptorSetLayout.Create(pLogicalDevice->GetHandle());

    m_SceneDescriptorPool.Create(pLogicalDevice->GetHandle(), &m_SceneDescriptorSetLayout, 1);
    m_SceneDescriptorPool.AllocateDescriptorSet();
    {
      u32 dstBinding = m_SceneDescriptorSetLayout.GetBindings()[0].binding;
      VkDescriptorType type = m_SceneDescriptorSetLayout.GetBindings()[0].descriptorType;
      m_SceneDescriptorPool.WriteBufferToDescriptorSet(m_BLASReferenceUniformBuffer.GetHandle(),
                                                       VK_WHOLE_SIZE,
                                                       dstBinding, type);
    }
    {
      u32 dstBinding = m_SceneDescriptorSetLayout.GetBindings()[1].binding;
      VkDescriptorType type = m_SceneDescriptorSetLayout.GetBindings()[1].descriptorType;
      m_SceneDescriptorPool.WriteBufferToDescriptorSet(m_LightUniformBuffer.GetHandle(),
                                                       VK_WHOLE_SIZE,
                                                       dstBinding, type);
    }

    // Creating ray tracing descriptors...
    m_RayTracingDescriptorSetLayout.AddBinding(VkDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
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
    m_RayTracingDescriptorSetLayout.Create(pLogicalDevice->GetHandle());

    m_RayTracingDescriptorPool.Create(pLogicalDevice->GetHandle(), &m_RayTracingDescriptorSetLayout, 1);
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
      m_RayTracingDescriptorPool.WriteBufferToDescriptorSet(m_PerFrameUniformBuffer.GetHandle(),
                                                            m_PerFrameUniformBuffer.GetFilledStride(),
                                                            dstBinding, type);
    }

    // Creating ray tracing pipeline...
    {
      VkDescriptorSetLayout setLayouts[]{ m_RayTracingDescriptorSetLayout.GetHandle(),
                                          m_SceneDescriptorSetLayout.GetHandle() };
      m_RayTracingPipelineLayout.Create(pLogicalDevice->GetHandle(), setLayouts);
    }

    {
      FPath shadersPath = FPath::Append(FPath::GetEngineProjectPath(), { "UGraphicsEngine", "Renderer", "Vulkan",
                                                                         "Shaders", "antialiasing_lambertian", "spv" });
      vulkan::FRayTracingPipelineSpecification rayTracingPipelineSpecification{
          .rayClosestHitPath = FPath::Append(shadersPath, "reflectionsrefractions.rchit.spv"),
          .rayGenerationPath = FPath::Append(shadersPath, "antialiasinglambertian.rgen.spv"),
          .rayMissPath =  FPath::Append(shadersPath, "antialiasinglambertian.rmiss.spv"),
          .rayShadowMissPath = FPath::Append(shadersPath, "antialiasinglambertian.rmiss.spv"),
          .pPipelineLayout = &m_RayTracingPipelineLayout,
          .pProperties = &pLogicalDevice->GetAttributes().GetRayTracingProperties(),
          .vkDevice = pLogicalDevice->GetHandle(),
          .pPhysicalDeviceAttributes = &pPhysicalDevice->GetAttributes(),
        .targetVulkanVersion = m_RenderContext.GetInstance()->GetAttributes().GetFullVersion()
      };
      m_RayTracingPipeline.Create(rayTracingPipelineSpecification);
    }

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

    // Destroying acceleration structures resources...
    for (vulkan::FBottomLevelAccelerationStructure& bottomAS : m_BottomLevelAccelerationVector)
    {
      bottomAS.Destroy();
    }
    m_TopLevelAS.Destroy();

    // Destroying descriptors...
    m_RayTracingDescriptorSetLayout.Destroy();
    m_RayTracingDescriptorPool.Destroy();

    m_SceneDescriptorSetLayout.Destroy();
    m_SceneDescriptorPool.Destroy();

    // Freeing buffers...
    m_PerFrameUniformBuffer.Free();
    m_BLASReferenceUniformBuffer.Free();
    m_LightUniformBuffer.Free();

    // Destroying pipelines...
    m_RayTracingPipelineLayout.Destroy();
    m_RayTracingPipeline.Destroy();

    // Closing renderer...
    m_Swapchain.Destroy();
    m_RenderContext.Destroy();

    // Destroying ECS and Asset systems...
    m_EntityRegistry.Destroy();
    m_AssetRegistry.Clear();

    // Destroying window...
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
                                      m_SceneDescriptorPool.GetDescriptorSet() };

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
  std::vector<vulkan::FBottomLevelAccelerationStructure> m_BottomLevelAccelerationVector{};
  vulkan::FTopLevelAccelerationStructure m_TopLevelAS{};
  vulkan::FBuffer m_BLASReferenceUniformBuffer{};
  vulkan::FDescriptorSetLayout m_RayTracingDescriptorSetLayout{};
  vulkan::FDescriptorPool m_RayTracingDescriptorPool{};
  vulkan::FPipelineLayout m_RayTracingPipelineLayout{};
  vulkan::FRayTracingPipeline m_RayTracingPipeline{};

  vulkan::FDescriptorSetLayout m_SceneDescriptorSetLayout{};
  vulkan::FDescriptorPool m_SceneDescriptorPool{};
  vulkan::FBuffer m_LightUniformBuffer{};

  FPerspectiveCamera m_Camera{};
  vulkan::FBuffer m_PerFrameUniformBuffer{};

  FAssetRegistry m_AssetRegistry{};
  FEntityRegistry m_EntityRegistry{};

  FLight m_Light{};

};


int main() {
  //return 0;
  Application app{};
  app.Run();

  return 0;
}
