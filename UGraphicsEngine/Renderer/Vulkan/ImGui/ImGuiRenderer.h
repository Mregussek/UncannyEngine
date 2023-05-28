
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
#include "UTools/Filesystem/Path.h"


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
  const FRenderPass* pRenderPass{ nullptr };
  u32 targetVulkanVersion{ UUNUSED };
};


class FImGuiRenderer
{
public:

  ~FImGuiRenderer();

  void Create(const FImGuiRendererSpecification& specification);
  void Destroy();

private:

  void CreateFontData(const FCommandPool& transferCommandPool, const FQueue& transferQueue);
  void CreateDescriptors();
  void CreatePipeline(const FImGuiRendererSpecification& specification);

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  FImage m_FontImage{};
  FSampler m_Sampler{};
  FBuffer m_VertexBuffer{};
  FBuffer m_IndexBuffer{};
  FDescriptorSetLayout m_DescriptorSetLayout{};
  FDescriptorPool m_DescriptorPool{};
  FPipelineLayout m_PipelineLayout{};
  FGraphicsPipeline m_GraphicsPipeline{};
  b8 m_IsCreated{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMGUIRENDERER_H
