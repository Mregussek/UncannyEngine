
#ifndef UNCANNYENGINE_IMGUIRENDERER_H
#define UNCANNYENGINE_IMGUIRENDERER_H


#include "imgui.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Image.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Sampler.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorSetLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Descriptors/DescriptorPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/GraphicsPipeline.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/RenderPass.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Semaphore.h"
#include "UTools/Filesystem/Path.h"
#include "UTools/Window/IWindow.h"


namespace uncanny::vulkan
{


struct FImGuiRendererSpecification
{
  FPath vertexShader{ UEMPTY_PATH };
  FPath fragmentShader{ UEMPTY_PATH };
  VkDevice vkDevice{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes{ nullptr };
  const FCommandPool* pTransferCommandPool{ nullptr };
  const FQueue* pTransferQueue{ nullptr };
  VkFormat swapchainFormat{ VK_FORMAT_UNDEFINED };
  FQueueFamilyIndex graphicsQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
  u32 backBufferCount{ UUNUSED };
  u32 targetVulkanVersion{ UUNUSED };
};


/// @brief FImGuiRenderer is a class that is responsible for rendering ImGui UI with Vulkan API.
/// It encapsulates all needed functionalities.
class FImGuiRenderer
{
public:

  FImGuiRenderer() = default;

  FImGuiRenderer(const FImGuiRenderer& other) = delete;
  FImGuiRenderer(FImGuiRenderer&& other) = delete;

  FImGuiRenderer& operator=(const FImGuiRenderer& other) = delete;
  FImGuiRenderer& operator=(FImGuiRenderer&& other) = delete;

  ~FImGuiRenderer();

public:

  void Create(const FImGuiRendererSpecification& specification);
  void Destroy();

  void BeginFrame(VkExtent2D swapchainExtent, FMouseButtonsPressed mouseButtonsPressed, FMousePosition mousePosition);
  void EndFrame(u32 frameIndex, const FQueue& queueUsingBuffers, VkFramebuffer swapchainFramebuffer);

// Getters
public:

  [[nodiscard]] VkSemaphore GetSemaphore(u32 frameIndex) const { return m_Semaphores[frameIndex].GetHandle(); }
  [[nodiscard]] const FCommandBuffer& GetCommandBuffer(u32 frameIndex) const { return m_CommandBuffers[frameIndex]; }
  [[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass.GetHandle(); }

// Private methods
private:

  void CreateFontData(const FCommandPool& transferCommandPool, const FQueue& transferQueue);
  void CreateDescriptors();
  void CreatePipeline(const FImGuiRendererSpecification& specification);

  void UpdateIO(VkExtent2D extent, FMouseButtonsPressed mouseButtonsPressed, FMousePosition mousePosition);
  void UpdateBuffers(const FQueue& queueUsingBuffers);

  void RecordRenderPass(u32 frameIndex, VkFramebuffer swapchainFramebuffer);
  void RecordDrawCommands(u32 frameIndex);

// Members
private:

  FImage m_FontImage{};
  FBuffer m_VertexBuffer{};
  FBuffer m_IndexBuffer{};

  FDescriptorSetLayout m_DescriptorSetLayout{};
  FDescriptorPool m_DescriptorPool{};

  FGraphicsPipeline m_GraphicsPipeline{};
  FCommandPool m_CommandPool{};

  std::vector<FCommandBuffer> m_CommandBuffers{};
  std::vector<FSemaphore> m_Semaphores{};

  FSampler m_Sampler{};

  FPipelineLayout m_PipelineLayout{};
  FRenderPass m_RenderPass{};

  VkDevice m_Device{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };

  b8 m_IsCreated{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMGUIRENDERER_H
