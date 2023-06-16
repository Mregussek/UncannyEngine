
#include "App.h"


Application::Application()
{
  CreateEngineResources();
  CreateLevelResources(m_ScenePaths[m_SelectedScenePath]);
}


Application::~Application()
{
  DestroyLevelResources();
  DestroyEngineResources();
}


void Application::Run() {
  while(not m_Window->IsGoingToClose())
  {
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
      m_CameraUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
    }

    m_Swapchain.WaitForNextImage();

    u32 frameIndex = m_Swapchain.GetCurrentFrameIndex();
    const vulkan::FQueue& graphicsQueue = m_RenderContext.GetLogicalDevice()->GetGraphicsQueue();

    m_ImGuiRenderer.BeginFrame(m_Swapchain.GetCurrentExtent(), m_Window->GetMouseButtonsPressed(),
                               m_Window->GetMousePosition());
    DrawImGui();
    m_ImGuiRenderer.EndFrame(frameIndex, graphicsQueue, m_Swapchain.GetFramebuffers()[frameIndex]);

    {
      VkSemaphore waitSemaphores[]{ m_Swapchain.GetImageAvailableSemaphore().GetHandle() };
      VkPipelineStageFlags waitStageFlags[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
      VkSemaphore signalSemaphores[]{ m_ImGuiRenderer.GetSemaphore(frameIndex) };
      graphicsQueue.Submit(waitSemaphores, waitStageFlags, m_CommandBuffers[frameIndex], signalSemaphores,
                           VK_NULL_HANDLE);
    }
    {
      VkSemaphore waitSemaphores[]{ m_ImGuiRenderer.GetSemaphore(frameIndex) };
      VkPipelineStageFlags waitStageFlags[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
      VkSemaphore signalSemaphores[]{ m_Swapchain.GetPresentableImageReadySemaphore().GetHandle() };
      VkFence fence{ m_Swapchain.GetFence().GetHandle() };
      graphicsQueue.Submit(waitSemaphores, waitStageFlags, m_ImGuiRenderer.GetCommandBuffer(frameIndex),
                           signalSemaphores, fence);
    }

    m_Swapchain.Present();

    if (m_Swapchain.IsOutOfDate() and m_Swapchain.IsRecreatePossible())
    {
      m_RenderContext.GetLogicalDevice()->WaitIdle();

      m_Swapchain.Recreate();

      m_OffscreenImage.Recreate(m_Swapchain.GetCurrentExtent());
      m_WriteOffscreenImageToDescriptorSet(m_OffscreenImage.GetHandleView());

      m_Camera.SetAspectRatio(m_Swapchain.GetCurrentAspectRatio());

      m_DepthImage.Recreate(m_Swapchain.GetCurrentExtent());

      m_Swapchain.CreateViews();
      m_Swapchain.CreateFramebuffers(m_ImGuiRenderer.GetRenderPass(), m_DepthImage.GetHandleView());

      RecordRayTracingCommands();
      m_Camera.ResetAccumulatedFrameCounter();
    }

    if (m_ShouldChangeScene)
    {
      m_RenderContext.GetLogicalDevice()->WaitIdle();
      DestroyLevelResources();
      CreateLevelResources(m_ScenePaths[m_SelectedScenePath]);
      m_Camera.ResetSpecification();
      m_Camera.ResetAccumulatedFrameCounter();
      m_Camera.DontAccumulatePreviousColors();
    }

    if (m_ShouldChangePipeline)
    {
      m_RenderContext.GetLogicalDevice()->WaitIdle();
      m_Camera.ResetAccumulatedFrameCounter();
      m_Camera.DontAccumulatePreviousColors();
      RecordRayTracingCommands();
    }
  }
}


void Application::DrawImGui()
{
  ImGui::SetNextWindowPos(ImVec2(2.5f, 2.5f));
  ImGui::SetNextWindowSize(ImVec2(500.f, 400.f), ImGuiCond_FirstUseEver);
  ImGui::Begin("Inspector Uncanny Engine Window");

  f32 frameRate = 1.f / m_Window->GetDeltaTime();
  ImGui::Text("FPS: %f", frameRate);

  u32 accumulatedFrames = m_Camera.GetAccumulatedFramesCounter();
  ImGui::Text("Accumulated Frames: %u", accumulatedFrames);

  ImGui::Separator();

  auto& rtxSpecs = m_Camera.GetRayTracingSpecification();

  bool dragged = UFALSE;
  {
    bool accumulate = rtxSpecs.accumulatePreviousColors;
    dragged |= ImGui::Checkbox("Accumulate Previous Colors", &accumulate);
    rtxSpecs.accumulatePreviousColors = accumulate;
  }
  dragged |= ImGui::DragInt("Accumulation Frames Limit", (i32*)&rtxSpecs.maxFrameCounterLimit, 1, 1, 8392);
  dragged |= ImGui::DragInt("Ray Bounces", (i32*)&rtxSpecs.maxRayBounces, 1, 1, 32);
  dragged |= ImGui::DragInt("Samples Per Pixel", (i32*)&rtxSpecs.maxSamplesPerPixel, 1, 1, 32);

  // Handling change scene...
  b32 changedSceneAndRemovedAccumulating =
      m_SelectedAccumulatedColor > rtxSpecs.accumulatePreviousColors and
      (m_ShouldChangeScene or m_ShouldChangePipeline);
  if (changedSceneAndRemovedAccumulating)
  {
    m_Camera.ContinueAccumulatingPreviousColors();
  }
  m_SelectedAccumulatedColor = rtxSpecs.accumulatePreviousColors;

  ImGui::Separator();

  {
    m_ShouldChangeScene = UFALSE;
    const i32 savedItem = m_SelectedScenePath;
    ImGui::Combo("Select Scene", &m_SelectedScenePath, m_ScenePathsCstr.data(), (i32)m_ScenePathsCstr.size());
    if (savedItem != m_SelectedScenePath)
    {
      m_ShouldChangeScene = UTRUE;
    }
  }

  ImGui::Separator();

  {
    m_ShouldChangePipeline = UFALSE;
    const i32 savedItem = m_SelectedRtxPipeline;
    ImGui::Combo("Select Pipeline", &m_SelectedRtxPipeline, m_RayTracingPipelinesCstr.data(),
                 (i32)m_RayTracingPipelinesCstr.size());
    if (savedItem != m_SelectedRtxPipeline)
    {
      m_ShouldChangePipeline = UTRUE;
    }
  }

  ImGui::Separator();

  auto& camSpecs = m_Camera.GetSpecification();
  ImGui::Text("Camera Position: (%f, %f, %f)", camSpecs.position.x, camSpecs.position.y, camSpecs.position.z);
  ImGui::Text("Camera Yaw: %f Pitch: %f", camSpecs.yaw, camSpecs.pitch);

  dragged |= ImGui::DragFloat("FoV", &camSpecs.fieldOfView, 0.5f, 1.f, 90.f);
  dragged |= ImGui::DragFloat("Near", &camSpecs.near, 0.01f, 0.01f, 2.f);
  dragged |= ImGui::DragFloat("Far", &camSpecs.far, 1.f, 1.f, 1000.f);
  ImGui::DragFloat("Movement Speed", &camSpecs.movementSpeed, 1.f, 1.f, 100.f);
  ImGui::DragFloat("Mouse Sensitivity", &camSpecs.sensitivity, 1.f, 50.f, 200.f);

  if (dragged)
  {
    m_Camera.ResetAccumulatedFrameCounter();
  }

  ImGui::End();
}


void Application::CreateEngineResources() {
  FLog::create();

  // Creating window...
  FWindowConfiguration windowConfiguration{
      .resizable = UTRUE,
      .fullscreen = UFALSE,
      .size = {
          .width = 1600,
          .height = 900
      },
      .name = "UncannyEngine Sandbox"
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
  constexpr u32 initialBackBufferCount = 2; // DO NOT USE IT LATER! Call m_Swapchain.GetBackBufferCount();
  m_Swapchain.Create(initialBackBufferCount, pLogicalDevice->GetHandle(), &pLogicalDevice->GetPresentQueue(),
                     m_RenderContext.GetWindowSurface());

  // Creating command pools...
  m_CommandPool.Create(pLogicalDevice->GetGraphicsFamilyIndex(),
                       pLogicalDevice->GetHandle(),
                       VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  // Creating command buffers...
  m_CommandBuffers = m_CommandPool.AllocatePrimaryCommandBuffers(m_Swapchain.GetBackBufferCount());

  // Creating camera...
  {
    FPerspectiveCameraSpecification cameraSpecification{
        .position = { -4.f, 0.f, 0.f },
        .front = { 0.f, 0.f, 0.f },
        .worldUp = { 0.f, 1.f, 0.f },
        .fieldOfView = 45.f,
        .aspectRatio = m_Swapchain.GetCurrentAspectRatio(),
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
        .maxRayBounces = 4,
        .maxSamplesPerPixel = 3,
        .accumulatePreviousColors = UTRUE
    };
    m_Camera.SetRayTracingSpecification(rayTracingSpecification);
    m_SelectedAccumulatedColor = m_Camera.GetRayTracingSpecification().accumulatePreviousColors;

    // Creating per frame buffer for camera...
    m_CameraUniformBuffer.Allocate(sizeof(FPerspectiveCameraUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   pLogicalDevice->GetHandle(), &pPhysicalDevice->GetAttributes());
    {
      FPerspectiveCameraUniformData uniformData = m_Camera.GetUniformData();
      m_CameraUniformBuffer.Fill(&uniformData, sizeof(FPerspectiveCameraUniformData), 1);
    }
  }

  // Creating off screen buffer...
  {
    VkImageUsageFlags flags =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    m_OffscreenImage.Allocate(m_Swapchain.GetFormat(), m_Swapchain.GetCurrentExtent(), flags,
                              VK_IMAGE_LAYOUT_PREINITIALIZED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {},
                              pLogicalDevice->GetHandle(), &pPhysicalDevice->GetAttributes());
    m_OffscreenImage.CreateView();
  }

  // Creating light buffer (currently not used in my shaders!)
  m_Light.position = { -1.f, -1.f, 0.2f };
  {
    FLightUniformData uniformData{ .position = m_Light.position };
    m_LightUniformBuffer.Allocate(sizeof(FLightUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  pLogicalDevice->GetHandle(), &pPhysicalDevice->GetAttributes());
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
    m_WriteBlasReferenceUniformToDescriptorSet = [this, dstBinding, type](VkBuffer bufferHandle)
    {
      m_SceneDescriptorPool.WriteBufferToDescriptorSet(bufferHandle, VK_WHOLE_SIZE, dstBinding, type);
    };
  }
  {
    u32 dstBinding = m_SceneDescriptorSetLayout.GetBindings()[1].binding;
    VkDescriptorType type = m_SceneDescriptorSetLayout.GetBindings()[1].descriptorType;
    VkBuffer bufferHandle = m_LightUniformBuffer.GetHandle();
    m_SceneDescriptorPool.WriteBufferToDescriptorSet(bufferHandle, VK_WHOLE_SIZE, dstBinding, type);
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
    m_WriteTlasToDescriptorSet = [this, dstBinding](VkAccelerationStructureKHR asHandle)
    {
      m_RayTracingDescriptorPool.WriteTopLevelAsToDescriptorSet(asHandle, dstBinding);
    };
  }
  {
    u32 dstBinding = m_RayTracingDescriptorSetLayout.GetBindings()[1].binding;
    m_WriteOffscreenImageToDescriptorSet = [this, dstBinding](VkImageView offscreenView)
    {
      m_RayTracingDescriptorPool.WriteStorageImageToDescriptorSet(offscreenView, dstBinding);
    };
    m_WriteOffscreenImageToDescriptorSet(m_OffscreenImage.GetHandleView());
  }
  {
    u32 dstBinding = m_RayTracingDescriptorSetLayout.GetBindings()[2].binding;
    VkDescriptorType type = m_RayTracingDescriptorSetLayout.GetBindings()[2].descriptorType;
    VkBuffer bufferHandle = m_CameraUniformBuffer.GetHandle();
    m_RayTracingDescriptorPool.WriteBufferToDescriptorSet(bufferHandle, VK_WHOLE_SIZE, dstBinding, type);
  }

  // Creating ray tracing pipeline...
  {
    VkDescriptorSetLayout setLayouts[]{ m_RayTracingDescriptorSetLayout.GetHandle(),
                                        m_SceneDescriptorSetLayout.GetHandle() };
    m_RayTracingPipelineLayout.Create(pLogicalDevice->GetHandle(), setLayouts);
  }
  {
    FPath shadersPath = FPath::Append(FPath::GetEngineProjectPath(), { "UGraphicsEngine", "Renderer", "Vulkan",
                                                                       "Shaders", "kajiya_path_tracing", "spv" });
    vulkan::FGLSLShaderCompiler glslCompiler{};
    glslCompiler.Initialize(m_RenderContext.GetInstance()->GetAttributes().GetFullVersion());

    vulkan::FRayTracingPipelineSpecification rayTracingPipelineSpecification{
        .rayClosestHitPath = FPath::Append(shadersPath, "pt_closesthit.rchit.spv"),
        .rayGenerationPath = FPath::Append(shadersPath, "pt_raygeneration.rgen.spv"),
        .rayMissPath =  FPath::Append(shadersPath, "pt_completemiss.rmiss.spv"),
        .rayShadowMissPath = FPath::Append(shadersPath, "pt_shadowmiss.rmiss.spv"),
        .pGlslCompiler = &glslCompiler,
        .pPipelineLayout = &m_RayTracingPipelineLayout,
        .pProperties = &pLogicalDevice->GetAttributes().GetRayTracingProperties(),
        .vkDevice = pLogicalDevice->GetHandle(),
        .pPhysicalDeviceAttributes = &pPhysicalDevice->GetAttributes()
    };
    m_RayTracingPipelines.reserve(3);   // When there will be reallocation program will fail!
    {
      rayTracingPipelineSpecification.rayClosestHitPath = FPath::Append(shadersPath, "pt_closesthit.rchit.spv");
      vulkan::FRayTracingPipeline& rtxPipeline = m_RayTracingPipelines.emplace_back();
      rtxPipeline.Create(rayTracingPipelineSpecification);
      m_RayTracingPipelinesCstr.push_back("Default");
    }
    {
      rayTracingPipelineSpecification.rayClosestHitPath = FPath::Append(shadersPath, "pt_normals_closesthit.rchit.spv");
      vulkan::FRayTracingPipeline& rtxPipeline = m_RayTracingPipelines.emplace_back();
      rtxPipeline.Create(rayTracingPipelineSpecification);
      m_RayTracingPipelinesCstr.emplace_back("Normals");
    }
    {
      rayTracingPipelineSpecification.rayClosestHitPath = FPath::Append(shadersPath,
                                                                        "pt_worldspacepos_closesthit.rchit.spv");
      vulkan::FRayTracingPipeline& rtxPipeline = m_RayTracingPipelines.emplace_back();
      rtxPipeline.Create(rayTracingPipelineSpecification);
      m_RayTracingPipelinesCstr.emplace_back("Hit World Space Positions");
    }
  }

  // Creating imgui
  {
    m_DepthImage.Allocate(VK_FORMAT_D32_SFLOAT, m_Swapchain.GetCurrentExtent(),
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                          VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {},
                          pLogicalDevice->GetHandle(), &pPhysicalDevice->GetAttributes());
    m_DepthImage.CreateView();

    FPath shadersPath = FPath::Append(FPath::GetEngineProjectPath(), { "UGraphicsEngine", "Renderer", "Vulkan",
                                                                       "Shaders", "imgui", "spv" });
    vulkan::FImGuiRendererSpecification imGuiRendererSpecification{
        .vertexShader = FPath::Append(shadersPath, "ui.vert.spv"),
        .fragmentShader = FPath::Append(shadersPath, "ui.frag.spv"),
        .vkDevice = pLogicalDevice->GetHandle(),
        .pPhysicalDeviceAttributes = &pPhysicalDevice->GetAttributes(),
        .pTransferCommandPool = &m_CommandPool,
        .pTransferQueue = &pLogicalDevice->GetGraphicsQueue(),
        .swapchainFormat = m_Swapchain.GetFormat(),
        .graphicsQueueFamilyIndex = pLogicalDevice->GetGraphicsFamilyIndex(),
        .backBufferCount = m_Swapchain.GetBackBufferCount(),
        .targetVulkanVersion = m_RenderContext.GetInstance()->GetAttributes().GetFullVersion()
    };

    m_ImGuiRenderer.Create(imGuiRendererSpecification);

    m_Swapchain.CreateViews();
    m_Swapchain.CreateFramebuffers(m_ImGuiRenderer.GetRenderPass(), m_DepthImage.GetHandleView());
  }

  // Adding scenes to change
  {
    FPath sceneSamplesPath = FPath::Append(FPath::GetEngineProjectPath(), { "USceneSamples" });
    m_ScenePaths.reserve(5);
    m_ScenePathsCstr.reserve(5);

    m_ScenePaths = FPath::GetFilePathsInDirectory(sceneSamplesPath);

    for (const FPath& scene : m_ScenePaths)
    {
      m_ScenePathsCstr.emplace_back(scene.GetString().c_str());
    }
  }
}


void Application::CreateLevelResources(const FPath& scenePath)
{
  const vulkan::FPhysicalDeviceAttributes& physicalDeviceAttributes =
      m_RenderContext.GetPhysicalDevice()->GetAttributes();
  const vulkan::FLogicalDevice* pLogicalDevice = m_RenderContext.GetLogicalDevice();

  // Registering entities with render mesh components and loading several obj files...
  m_EntityRegistry.Create();
  FEntityRegistryLoader::LoadJsonScene(scenePath.GetString().c_str(), &m_EntityRegistry, &m_AssetRegistry);

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
                   pLogicalDevice->GetHandle(), &physicalDeviceAttributes);
  }
  m_TopLevelAS.Build(m_BottomLevelAccelerationVector, m_CommandPool, pLogicalDevice->GetGraphicsQueue(),
                     pLogicalDevice->GetHandle(), &physicalDeviceAttributes);
  m_WriteTlasToDescriptorSet(m_TopLevelAS.GetHandle());

  // Creating blas reference uniform buffer...
  const auto& blasUniformData = m_TopLevelAS.GetBLASReferenceUniformData();
  m_BLASReferenceUniformBuffer.Allocate(blasUniformData.size() * sizeof(blasUniformData[0]),
                                        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pLogicalDevice->GetHandle(),
                                        &physicalDeviceAttributes);
  m_BLASReferenceUniformBuffer.FillStaged(blasUniformData.data(), sizeof(blasUniformData[0]),
                                          blasUniformData.size(), m_CommandPool,
                                          pLogicalDevice->GetGraphicsQueue());
  m_WriteBlasReferenceUniformToDescriptorSet(m_BLASReferenceUniformBuffer.GetHandle());

  // Recording commands
  RecordRayTracingCommands();
}


void Application::DestroyLevelResources()
{
  if (m_RenderContext.GetLogicalDevice()->IsValid())
  {
    m_RenderContext.GetLogicalDevice()->WaitIdle();
  }

  // Destroying bottom level acceleration structures...
  for (vulkan::FBottomLevelAccelerationStructure& bottomAS : m_BottomLevelAccelerationVector)
  {
    bottomAS.Destroy();
  }
  m_BottomLevelAccelerationVector.clear();

  // Destroying top level acceleration structure...
  m_TopLevelAS.Destroy();

  // Free blas reference uniform buffer...
  m_BLASReferenceUniformBuffer.Free();

  // Destroying ECS...
  m_EntityRegistry.Destroy();

  // Destroying asset system...
  m_AssetRegistry.Clear();
}


void Application::DestroyEngineResources()
{
  if (m_RenderContext.GetLogicalDevice()->IsValid())
  {
    m_RenderContext.GetLogicalDevice()->WaitIdle();
  }

  // Closing imgui
  m_ImGuiRenderer.Destroy();
  m_DepthImage.Free();

  // Closing render target images...
  m_OffscreenImage.Free();

  // Closing command buffers...
  for (vulkan::FCommandBuffer& cmdBuf : m_CommandBuffers)
  {
    cmdBuf.Free();
  }
  m_CommandBuffers.clear();

  // Closing Command Pools...
  m_CommandPool.Destroy();

  // Destroying descriptors...
  m_RayTracingDescriptorSetLayout.Destroy();
  m_RayTracingDescriptorPool.Destroy();

  m_SceneDescriptorSetLayout.Destroy();
  m_SceneDescriptorPool.Destroy();

  // Freeing buffers...
  m_CameraUniformBuffer.Free();
  m_LightUniformBuffer.Free();

  // Destroying pipelines...
  m_RayTracingPipelineLayout.Destroy();
  for (vulkan::FRayTracingPipeline& rtxPipeline : m_RayTracingPipelines)
  {
    rtxPipeline.Destroy();
  }

  // Closing renderer...
  m_Swapchain.Destroy();
  m_RenderContext.Destroy();

  // Destroying window...
  m_Window->Destroy();

  // imgui.ini causes app a crash, that is why I decided to delete at the end :/
  DeleteImGuiIni();
}

void Application::RecordRayTracingCommands()
{
  m_CommandPool.Reset();

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
  vulkan::FRayTracingPipeline& rtxPipeline = m_RayTracingPipelines[m_SelectedRtxPipeline];

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
    cmdBuf.BindPipeline(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtxPipeline.GetHandle());
    cmdBuf.BindDescriptorSets(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RayTracingPipelineLayout.GetHandle(),
                              descriptorSets);
    cmdBuf.TraceRays(&rtxPipeline, offscreenExtent);
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


void Application::DeleteImGuiIni()
{
  FPath imgui_ini = FPath::Append(FPath::GetExecutablePath(), "imgui.ini");
  FPath::Delete(imgui_ini);
}