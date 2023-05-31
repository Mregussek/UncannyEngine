
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

  void Start();

  void Destroy();

  void RecordCommands();

  void DeleteImGuiIni();

private:

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

  vulkan::FImage m_DepthImage{};
  vulkan::FImGuiRenderer m_ImGuiRenderer{};

  FPerspectiveCamera m_Camera{};
  vulkan::FBuffer m_PerFrameUniformBuffer{};

  FAssetRegistry m_AssetRegistry{};
  FEntityRegistry m_EntityRegistry{};

  FLight m_Light{};

};


#endif //UNCANNYENGINE_APP_H
