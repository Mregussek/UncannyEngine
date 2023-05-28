
#include "ImGuiRenderer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"
#include "UMath/Vector2.h"


namespace uncanny::vulkan
{


struct FPushConstBlock {
  math::Vector2f scale;
  math::Vector2f translate;
} pushConstBlock;


FImGuiRenderer::~FImGuiRenderer()
{
  if (m_IsCreated)
  {
    Destroy();
  }
}


void FImGuiRenderer::Create(const FImGuiRendererSpecification& specification)
{
  m_Device = specification.vkDevice;
  m_pPhysicalDeviceAttributes = specification.pPhysicalDeviceAttributes;

  ImGui::CreateContext();
  CreateFontData(*specification.pTransferCommandPool, *specification.pTransferQueue);
  CreateDescriptors();
  CreatePipeline(specification);

  m_IsCreated = UTRUE;
}


void FImGuiRenderer::CreateFontData(const FCommandPool& transferCommandPool, const FQueue& transferQueue)
{
  ImGuiIO& io = ImGui::GetIO();

  // Create font texture
  unsigned char* fontData{ nullptr };
  i32 texWidth{ -1 };
  i32 texHeight{ -1 };
  io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

  // Create font image for copy
  VkExtent2D fontExtent{ .width = (u32)texWidth, .height = (u32)texHeight };
  FQueueFamilyIndex queueFamilyIndex[]{ VK_QUEUE_FAMILY_IGNORED };
  m_FontImage = FImage(m_Device, m_pPhysicalDeviceAttributes);
  m_FontImage.Allocate(VK_FORMAT_R8G8B8A8_UNORM, fontExtent,
                       VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                       VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       queueFamilyIndex);
  m_FontImage.CreateView();

  // Staging buffer for font uploading
  VkDeviceSize stagingSize = texWidth * texHeight * 4 * sizeof(char);
  FBuffer stagingBuffer{ m_Device, m_pPhysicalDeviceAttributes };
  stagingBuffer.Allocate(stagingSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  stagingBuffer.Fill(fontData, 4 * sizeof(char), texWidth * texHeight);

  // Copy Buffer Into Image
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
  FCommandBuffer commandBuffer = transferCommandPool.AllocatePrimaryCommandBuffer();
  commandBuffer.BeginOneTimeRecording();
  commandBuffer.ImageMemoryBarrier(m_FontImage.GetHandle(),
                                   VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   subresourceRange,
                                   VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
  commandBuffer.CopyBufferToImage(stagingBuffer.GetHandle(), m_FontImage.GetHandle(), subresourceLayers,
                                  fontExtent);
  commandBuffer.ImageMemoryBarrier(m_FontImage.GetHandle(),
                                   VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   subresourceRange,
                                   VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
  commandBuffer.EndRecording();

  VkPipelineStageFlags waitStageFlags[]{ commandBuffer.GetLastWaitPipelineStage() };
  transferQueue.Submit({}, waitStageFlags, commandBuffer, {}, VK_NULL_HANDLE);
  transferQueue.WaitIdle();

  m_Sampler = FSampler(m_Device);
  m_Sampler.Create();
}


void FImGuiRenderer::CreateDescriptors()
{
  m_DescriptorSetLayout.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
                                   nullptr);
  m_DescriptorSetLayout.Create(m_Device);

  m_DescriptorPool.Create(m_Device, &m_DescriptorSetLayout, 2);
  m_DescriptorPool.AllocateDescriptorSet();
  m_DescriptorPool.WriteSamplerToDescriptorSet(m_Sampler.GetHandle(), m_FontImage.GetHandleView(),
                                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);
}


void FImGuiRenderer::CreatePipeline(const FImGuiRendererSpecification& specification)
{
  VkPushConstantRange pushConstantRange{
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .offset = 0,
    .size = sizeof(FPushConstBlock)
  };
  m_PipelineLayout.Create(m_Device, m_DescriptorSetLayout.GetHandle(), pushConstantRange);

  VkVertexInputBindingDescription vertexInputBindingDescription[]{
      VkVertexInputBindingDescription{
        .binding = 0,
        .stride = sizeof(ImDrawVert),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
      }
  };
  VkVertexInputAttributeDescription vertexInputAttributeDescription[]{
      VkVertexInputAttributeDescription{
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = (u32)offsetof(ImDrawVert, pos)
      },
      VkVertexInputAttributeDescription{
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = (u32)offsetof(ImDrawVert, uv)
      },
      VkVertexInputAttributeDescription{
        .location = 2,
        .binding = 0,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .offset = (u32)offsetof(ImDrawVert, col)
      }
  };

  FGraphicsPipelineSpecification pipelineSpecification{
    .vertexInputBindingDescriptions = vertexInputBindingDescription,
    .vertexInputAttributeDescriptions = vertexInputAttributeDescription,
    .vertexShader = specification.vertexShader,
    .fragmentShader = specification.fragmentShader,
    .pipelineLayout = m_PipelineLayout.GetHandle(),
    .renderPass = specification.pRenderPass->GetHandle(),
    .vkDevice = m_Device,
    .targetVulkanVersion = specification.targetVulkanVersion
  };

  m_GraphicsPipeline.Create(pipelineSpecification);
}


void FImGuiRenderer::Destroy()
{
  ImGui::DestroyContext();
  m_VertexBuffer.Free();
  m_IndexBuffer.Free();
  m_FontImage.Free();
  m_Sampler.Destroy();
  m_DescriptorSetLayout.Destroy();
  m_DescriptorPool.Destroy();
  m_PipelineLayout.Destroy();
  m_GraphicsPipeline.Destroy();
  m_IsCreated = UFALSE;
}


}
