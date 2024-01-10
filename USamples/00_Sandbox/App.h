
#ifndef UNCANNYENGINE_APP_H
#define UNCANNYENGINE_APP_H


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
#include "UGraphicsEngine/Renderer/Vulkan/ImGui/ImGuiRenderer.h"
#include "UGraphicsEngine/Renderer/PerspectiveCamera.h"
#include "UGraphicsEngine/Renderer/RenderMesh.h"
#include "UGraphicsEngine/Renderer/Light.h"


using namespace uncanny;


class Application
{

public:

  Application();

  ~Application();

  void Run();

private:

  void CreateEngineResources();

  void CreateLevelResources(const FPath& scenePath);
  void DestroyLevelResources();

  void DestroyEngineResources();

  void RecordRayTracingCommands();

  void DrawImGui();

  void DeleteImGuiIni();

  void CalculateStatisticsForLevelResources(const std::vector<FRenderData>& renderDataVector);

private:

  // Engine Resources that are initialized once and then reused...

  std::shared_ptr<IWindow> m_Window;
  vulkan::FRenderContext m_RenderContext{};
  vulkan::FSwapchain m_Swapchain{};

  vulkan::FCommandPool m_CommandPool{};
  std::vector<vulkan::FCommandBuffer> m_CommandBuffers{};

  FPerspectiveRayTracingCamera m_Camera{};
  vulkan::FBuffer m_CameraUniformBuffer{};

  vulkan::FImage m_OffscreenImage{};

  vulkan::FDescriptorSetLayout m_SceneDescriptorSetLayout{};
  vulkan::FDescriptorPool m_SceneDescriptorPool{};
  std::function<void(VkBuffer)> m_WriteBlasReferenceUniformToDescriptorSet{};

  vulkan::FDescriptorSetLayout m_RayTracingDescriptorSetLayout{};
  vulkan::FDescriptorPool m_RayTracingDescriptorPool{};
  std::function<void(VkImageView)> m_WriteOffscreenImageToDescriptorSet{};
  std::function<void(VkAccelerationStructureKHR)> m_WriteTlasToDescriptorSet{};

  vulkan::FPipelineLayout m_RayTracingPipelineLayout{};
  std::vector<vulkan::FRayTracingPipeline> m_RayTracingPipelines{};
  std::vector<const char*> m_RayTracingPipelinesCstr{};

  vulkan::FImage m_DepthImage{};
  vulkan::FImGuiRenderer m_ImGuiRenderer{};

  std::vector<FPath> m_ScenePaths{};
  std::vector<const char*> m_ScenePathsCstr{};

  // Level Resources that can be reinitialized when scene is changed at runtime

  std::vector<vulkan::FBottomLevelAccelerationStructure> m_BottomLevelAccelerationVector{};
  vulkan::FTopLevelAccelerationStructure m_TopLevelAS{};
  vulkan::FBuffer m_BLASReferenceUniformBuffer{};

  FAssetRegistry m_AssetRegistry{};
  FEntityRegistry m_EntityRegistry{};

  i32 m_SelectedScenePath{ 0 };
  b32 m_ShouldChangeScene{ UFALSE };

  i32 m_SelectedRtxPipeline{ 0 };
  b32 m_ShouldChangePipeline{ UFALSE };

  b32 m_SelectedAccumulatedColor{ UFALSE };

  struct LevelStatistics
  {
    u32 bottomLevelStructsCount{ 0 };
    u32 allTrianglesCount{ 0 };
    u32 allVerticesCount{ 0 };
    u32 allIndicesCount{ 0 };
  };

  LevelStatistics m_LevelStats{};

};


#endif //UNCANNYENGINE_APP_H
