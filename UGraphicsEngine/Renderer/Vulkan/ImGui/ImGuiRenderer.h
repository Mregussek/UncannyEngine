
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
  FPath vertexShader{ "" };
  FPath fragmentShader{ "" };
  VkDevice vkDevice{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes{ nullptr };
  const FCommandPool* pTransferCommandPool{ nullptr };
  const FQueue* pTransferQueue{ nullptr };
  VkFormat swapchainFormat{ VK_FORMAT_UNDEFINED };
  vulkan::FQueueFamilyIndex graphicsQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
  u32 backBufferCount{ UUNUSED };
  u32 targetVulkanVersion{ UUNUSED };
};


class FImGuiRenderer
{
public:

  ~FImGuiRenderer();

  void Create(const FImGuiRendererSpecification& specification);
  void Destroy();

  void Update(u32 frameIndex, VkFramebuffer swapchainFramebuffer, VkExtent2D swapchainExtent,
              FMouseButtonsPressed mouseButtonsPressed, FMousePosition mousePosition);

  [[nodiscard]] VkSemaphore GetSemaphore(u32 frameIndex) const { return m_Semaphores[frameIndex].GetHandle(); }
  [[nodiscard]] const FCommandBuffer& GetCommandBuffer(u32 frameIndex) const { return m_CommandBuffers[frameIndex]; }
  [[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass.GetHandle(); }

private:

  void CreateFontData(const FCommandPool& transferCommandPool, const FQueue& transferQueue);
  void CreateDescriptors();
  void CreatePipeline(const FImGuiRendererSpecification& specification);

  void UpdateIO(VkExtent2D extent, FMouseButtonsPressed mouseButtonsPressed, FMousePosition mousePosition);
  void UpdateBuffers(u32 frameIndex);

  void RecordRenderPass(u32 frameIndex, VkFramebuffer swapchainFramebuffer, VkExtent2D swapchainExtent);
  void RecordDrawCommands(u32 frameIndex, const FCommandBuffer& commandBuffer);

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  std::vector<FSemaphore> m_Semaphores{};
  FImage m_FontImage{};
  FSampler m_Sampler{};
  std::vector<FBuffer> m_VertexBuffers{};
  std::vector<FBuffer> m_IndexBuffers{};
  FDescriptorSetLayout m_DescriptorSetLayout{};
  FDescriptorPool m_DescriptorPool{};
  FPipelineLayout m_PipelineLayout{};
  vulkan::FRenderPass m_RenderPass{};
  FGraphicsPipeline m_GraphicsPipeline{};
  FCommandPool m_CommandPool{};
  std::vector<FCommandBuffer> m_CommandBuffers{};
  u32 m_VertexCount{ 0 };
  u32 m_IndexCount{ 0 };
  b8 m_IsCreated{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMGUIRENDERER_H
