
#include <array>
#include "ImGuiRenderer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandBuffer.h"
#include "UMath/Vector2.h"
#include "UTools/Logger/Log.h"


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

  m_Semaphores.resize(specification.backBufferCount);
  for(u32 i = 0; i < specification.backBufferCount; i++)
  {
    m_Semaphores[i].Create(m_Device);
  }

  m_RenderPass.Create(specification.swapchainFormat, VK_FORMAT_D32_SFLOAT, m_Device);
  m_CommandPool.Create(specification.graphicsQueueFamilyIndex, m_Device,
                       VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  m_CommandBuffers = m_CommandPool.AllocatePrimaryCommandBuffers(specification.backBufferCount);

  ImGui::CreateContext();

  CreateFontData(*specification.pTransferCommandPool, *specification.pTransferQueue);
  CreateDescriptors();
  CreatePipeline(specification);

  m_IsCreated = UTRUE;
}


void FImGuiRenderer::Destroy()
{
  ImGui::DestroyContext();
  for(FSemaphore& semaphore : m_Semaphores)
  {
    semaphore.Destroy();
  }
  for(FCommandBuffer& commandBuffer : m_CommandBuffers)
  {
    commandBuffer.Free();
  }
  m_VertexBuffer.Free();
  m_IndexBuffer.Free();
  m_CommandPool.Destroy();
  m_RenderPass.Destroy();
  m_FontImage.Free();
  m_Sampler.Destroy();
  m_DescriptorSetLayout.Destroy();
  m_DescriptorPool.Destroy();
  m_PipelineLayout.Destroy();
  m_GraphicsPipeline.Destroy();
  m_IsCreated = UFALSE;
}


void FImGuiRenderer::Update(u32 frameIndex, const FQueue& queueUsingBuffers, VkFramebuffer swapchainFramebuffer,
                            VkExtent2D swapchainExtent, FMouseButtonsPressed mouseButtonsPressed,
                            FMousePosition mousePosition)
{
  UpdateIO(swapchainExtent, mouseButtonsPressed, mousePosition);

  ImGui::NewFrame();

  //ImGui::SetNextWindowPos(ImVec2(10.f, 10.f));
  ImGui::SetNextWindowSize(ImVec2(100.f, 100.f), ImGuiCond_FirstUseEver);

  ImGui::Begin("Vulkan Example");
  ImGui::Text("Mateusz Rzeczyca");
  ImGui::End();

  //ImGui::ShowDemoWindow();

  // Render to generate draw buffers
  ImGui::Render();

  UpdateBuffers(queueUsingBuffers);
  RecordRenderPass(frameIndex, swapchainFramebuffer, swapchainExtent);
}


void FImGuiRenderer::UpdateIO(VkExtent2D extent, FMouseButtonsPressed mouseButtonsPressed,
                              FMousePosition mousePosition)
{
  auto ConvertToRange = []<typename T>(T oldValue, T oldMin, T oldMax, T newMin, T newMax)
  {
    return ((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin) + newMin;
  };

  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((f32)extent.width, (f32)extent.height);
  io.MousePos = ImVec2(ConvertToRange((f32)mousePosition.x, 0.f, 1.f, 0.f, (f32)extent.width),
                       ConvertToRange((f32)mousePosition.y, 0.f, 1.f, 0.f, (f32)extent.height));
  io.MouseDown[0] = mouseButtonsPressed.left;
  io.MouseDown[1] = mouseButtonsPressed.right;
}


void FImGuiRenderer::UpdateBuffers(const FQueue& queueUsingBuffers)
{
  ImDrawData* imDrawData = ImGui::GetDrawData();
  if (not imDrawData)
  {
    return;
  }

  VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
  VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

  if ((vertexBufferSize == 0) or (indexBufferSize == 0))
  {
    return;
  }

  if ((not m_VertexBuffer.IsValid()) or (vertexBufferSize > m_VertexBuffer.GetAllocatedSize()))
  {
    queueUsingBuffers.WaitIdle();
    m_VertexBuffer.Free();
    m_VertexBuffer.Allocate(vertexBufferSize * 2, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_Device, m_pPhysicalDeviceAttributes);
  }
  if ((not m_IndexBuffer.IsValid()) or (indexBufferSize > m_IndexBuffer.GetAllocatedSize()))
  {
    queueUsingBuffers.WaitIdle();
    m_IndexBuffer.Free();
    m_IndexBuffer.Allocate(indexBufferSize * 2, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_Device, m_pPhysicalDeviceAttributes);
  }

  {
    auto* pVertexDst = static_cast<ImDrawVert*>(m_VertexBuffer.Map());
    auto* pIndexDst = static_cast<ImDrawIdx*>(m_IndexBuffer.Map());

    std::span<ImDrawList*> cmdLists{ imDrawData->CmdLists, (u32)imDrawData->CmdListsCount };
    for (const ImDrawList* pCmdList : cmdLists)
    {
      memcpy(pVertexDst, pCmdList->VtxBuffer.Data, pCmdList->VtxBuffer.Size * sizeof(ImDrawVert));
      memcpy(pIndexDst, pCmdList->IdxBuffer.Data, pCmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
      pVertexDst += pCmdList->VtxBuffer.Size;
      pIndexDst += pCmdList->IdxBuffer.Size;
    }

    m_VertexBuffer.Unmap();
    m_IndexBuffer.Unmap();
  }
}


void FImGuiRenderer::RecordRenderPass(u32 frameIndex, VkFramebuffer swapchainFramebuffer, VkExtent2D swapchainExtent)
{
  VkRect2D renderArea{ .offset = { .x = 0, .y = 0 }, .extent = swapchainExtent };
  std::array<VkClearValue, 2> clearValues{};
  FCommandBuffer& commandBuffer = m_CommandBuffers[frameIndex];

  commandBuffer.BeginRecording();

  VkRenderPassBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_RenderPass.GetHandle(),
      .framebuffer = swapchainFramebuffer,
      .renderArea = renderArea,
      .clearValueCount = clearValues.size(),
      .pClearValues = clearValues.data()
  };
  vkCmdBeginRenderPass(commandBuffer.GetHandle(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

  RecordDrawCommands(frameIndex);

  vkCmdEndRenderPass(commandBuffer.GetHandle());
  commandBuffer.EndRecording();
}


void FImGuiRenderer::RecordDrawCommands(u32 frameIndex)
{
  ImGuiIO& io = ImGui::GetIO();
  FCommandBuffer& commandBuffer = m_CommandBuffers[frameIndex];

  VkDescriptorSet descriptorSet = m_DescriptorPool.GetDescriptorSet();
  vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout.GetHandle(), 0,
                          1, &descriptorSet,
                          0, nullptr);
  vkCmdBindPipeline(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline.GetHandle());

  VkViewport viewport{ .width = io.DisplaySize.x, .height = io.DisplaySize.y, .minDepth = 0.0f, .maxDepth = 1.f };
  vkCmdSetViewport(commandBuffer.GetHandle(), 0, 1, &viewport);

  pushConstBlock = FPushConstBlock{
    .scale = math::Vector2f{ 2.f / io.DisplaySize.x, 2.f / io.DisplaySize.y },
    .translate = math::Vector2f{ -1.f, -1.f }
  };
  vkCmdPushConstants(commandBuffer.GetHandle(), m_PipelineLayout.GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                     sizeof(FPushConstBlock), &pushConstBlock);

  VkBuffer vertexHandle = m_VertexBuffer.GetHandle();
  std::span<VkBuffer> vertexHandles{ &vertexHandle, 1 };
  VkDeviceSize offsets[]{ 0 };

  vkCmdBindVertexBuffers(commandBuffer.GetHandle(), 0, vertexHandles.size(), vertexHandles.data(), offsets);
  vkCmdBindIndexBuffer(commandBuffer.GetHandle(), m_IndexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT16);

  ImDrawData* imDrawData = ImGui::GetDrawData();
  i32 vertexOffset = 0;
  u32 indexOffset = 0;

  std::span<ImDrawList*> commandsLists{ imDrawData->CmdLists, (u32)imDrawData->CmdListsCount };
  for (const ImDrawList* pCmdList : commandsLists)
  {
    std::span<ImDrawCmd> commands{ pCmdList->CmdBuffer.Data, (u32)pCmdList->CmdBuffer.Size };
    for (const ImDrawCmd& pCmd : commands)
    {
      VkRect2D scissorRect{
          .offset = VkOffset2D{
              .x = std::max((i32)(pCmd.ClipRect.x), 0),
              .y = std::max((i32)(pCmd.ClipRect.y), 0)
          },
          .extent = VkExtent2D{
              .width = (u32)(pCmd.ClipRect.z - pCmd.ClipRect.x),
              .height = (u32)(pCmd.ClipRect.w - pCmd.ClipRect.y)
          }
      };
      vkCmdSetScissor(commandBuffer.GetHandle(), 0, 1, &scissorRect);
      vkCmdDrawIndexed(commandBuffer.GetHandle(), pCmd.ElemCount, 1, indexOffset, vertexOffset, 0);
      indexOffset += pCmd.ElemCount;
    }
    vertexOffset += pCmdList->VtxBuffer.Size;
  }
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

  m_FontImage.Allocate(VK_FORMAT_R8G8B8A8_UNORM, fontExtent,
                       VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                       VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {},
                       m_Device, m_pPhysicalDeviceAttributes);
  m_FontImage.CreateView();

  // Staging buffer for font uploading
  VkDeviceSize stagingSize = texWidth * texHeight * 4 * sizeof(char);
  FBuffer stagingBuffer{};
  stagingBuffer.Allocate(stagingSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         m_Device, m_pPhysicalDeviceAttributes);
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

  m_Sampler.Create(m_Device);
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
      .renderPass = GetRenderPass(),
      .vkDevice = m_Device,
      .targetVulkanVersion = specification.targetVulkanVersion
  };

  m_GraphicsPipeline.Create(pipelineSpecification);
}


}
