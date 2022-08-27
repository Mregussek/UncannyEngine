
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createRenderPasses() {
  UTRACE("Creating render passes...");

  // TODO: Find better way for automating format retrieval for render pass
  UWARN("During render pass creation using 0-indexed render target format from dependencies!");
  VkFormat renderPassFormat{ mImageDependencies.renderTarget.formatCandidatesVector[0].format };

  VkAttachmentDescription colorAttachDesc{};
  colorAttachDesc.flags = 0;
  colorAttachDesc.format = renderPassFormat;
  colorAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

  VkAttachmentReference colorAttachRef{};
  colorAttachRef.attachment = 0;
  colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDesc{};
  subpassDesc.flags = 0;
  subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDesc.inputAttachmentCount = 0;
  subpassDesc.pInputAttachments = nullptr;
  subpassDesc.colorAttachmentCount = 1;
  subpassDesc.pColorAttachments = &colorAttachRef;
  subpassDesc.pResolveAttachments = nullptr;
  subpassDesc.pDepthStencilAttachment = nullptr;
  subpassDesc.preserveAttachmentCount = 0;
  subpassDesc.pPreserveAttachments = nullptr;

  VkRenderPassCreateInfo createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.attachmentCount = 1;
  createInfo.pAttachments = &colorAttachDesc;
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpassDesc;
  createInfo.dependencyCount = 0;
  createInfo.pDependencies = nullptr;

  VkResult created{ vkCreateRenderPass(mVkDevice, &createInfo, nullptr, &mVkRenderPass) };
  if (created != VK_SUCCESS) {
    UERROR("Could not create render pass! VkResult: {}", created);
    return UFALSE;
  }

  UDEBUG("Created render passes!");
  return UTRUE;
}

b32 FRenderContextVulkan::closeRenderPasses() {
  UTRACE("Closing render passes...");

  if (mVkRenderPass != VK_NULL_HANDLE) {
    UTRACE("Destroying render pass...");
    vkDestroyRenderPass(mVkDevice, mVkRenderPass, nullptr);
  }
  else {
    UWARN("As render pass is not created, it won't be destroyed!");
  }

  UDEBUG("Closed render passes!");
  return UTRUE;
}



}
