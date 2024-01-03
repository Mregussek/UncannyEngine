
#include "RenderPass.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include <array>


namespace uncanny::vulkan
{


FRenderPass::~FRenderPass()
{
  Destroy();
}


void FRenderPass::Create(b32 clearColorAttachmentEveryDraw, VkFormat swapchainFormat, VkFormat depthFormat,
                         VkDevice vkDevice)
{
  m_Device = vkDevice;

  std::array<VkAttachmentDescription, 2> attachments{
      VkAttachmentDescription{
        .flags = 0,
        .format = swapchainFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      },
      VkAttachmentDescription{
        .flags = 0,
        .format = depthFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
      }
  };

  if (clearColorAttachmentEveryDraw)
  {
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  }

  VkAttachmentReference colorReference{
    .attachment = 0,
    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
  };
  VkAttachmentReference depthReference{
    .attachment = 1,
    .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
  };

  VkSubpassDescription subpassDescription{
    .flags = 0,
    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
    .inputAttachmentCount = 0,
    .pInputAttachments = nullptr,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorReference,
    .pResolveAttachments = nullptr,
    .pDepthStencilAttachment = &depthReference,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = nullptr
  };

  std::array<VkSubpassDependency, 2> subpassDependencies{
      VkSubpassDependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
      },
      VkSubpassDependency{
        .srcSubpass = 0,
        .dstSubpass = VK_SUBPASS_EXTERNAL,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
      }
  };

  VkRenderPassCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .attachmentCount = attachments.size(),
    .pAttachments = attachments.data(),
    .subpassCount = 1,
    .pSubpasses = &subpassDescription,
    .dependencyCount = subpassDependencies.size(),
    .pDependencies = subpassDependencies.data()
  };

  VkResult result = vkCreateRenderPass(m_Device, &createInfo, nullptr, &m_RenderPass);
  AssertVkAndThrow(result);
}


void FRenderPass::Destroy()
{
  if (m_RenderPass != VK_NULL_HANDLE)
  {
    vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    m_RenderPass = VK_NULL_HANDLE;
  }
}


}
