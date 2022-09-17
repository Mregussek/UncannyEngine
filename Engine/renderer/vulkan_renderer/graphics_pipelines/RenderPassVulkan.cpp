
#include "RenderPassVulkan.h"
#include <utilities/Logger.h>
#include <array>


namespace uncanny
{


b32 FRenderPassVulkan::create(const FRenderPassCreateDependenciesVulkan& deps) {
  UTRACE("Creating render pass {}...", deps.logInfo);
  mData.logInfo = deps.logInfo;
  mData.renderTargetFormat = deps.renderTargetFormat;
  mData.depthFormat = deps.depthFormat;

  VkAttachmentDescription colorAttachDesc{};
  colorAttachDesc.flags = 0;
  colorAttachDesc.format = mData.renderTargetFormat;
  colorAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

  VkAttachmentDescription depthAttachDesc{};
  depthAttachDesc.flags = 0;
  depthAttachDesc.format = mData.depthFormat;
  depthAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  std::array<VkAttachmentDescription, 2> attachDesc{ colorAttachDesc, depthAttachDesc };

  VkAttachmentReference colorAttachRef{};
  colorAttachRef.attachment = 0;
  colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachRef{};
  depthAttachRef.attachment = 1;
  depthAttachRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDesc{};
  subpassDesc.flags = 0;
  subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDesc.inputAttachmentCount = 0;
  subpassDesc.pInputAttachments = nullptr;
  subpassDesc.colorAttachmentCount = 1;
  subpassDesc.pColorAttachments = &colorAttachRef;
  subpassDesc.pResolveAttachments = nullptr;
  subpassDesc.pDepthStencilAttachment = &depthAttachRef;
  subpassDesc.preserveAttachmentCount = 0;
  subpassDesc.pPreserveAttachments = nullptr;

  VkSubpassDependency subpassDeps{};
  subpassDeps.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDeps.dstSubpass = 0;
  subpassDeps.srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpassDeps.dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpassDeps.srcAccessMask = 0;
  subpassDeps.dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  subpassDeps.dependencyFlags = 0;

  VkRenderPassCreateInfo createInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.attachmentCount = attachDesc.size();
  createInfo.pAttachments = attachDesc.data();
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpassDesc;
  createInfo.dependencyCount = 1;
  createInfo.pDependencies = &subpassDeps;

  VkResult created{ vkCreateRenderPass(deps.device, &createInfo, nullptr,
                                       &mData.renderPass) };
  if (created != VK_SUCCESS) {
    UERROR("Could not create render pass {}! VkResult: {}", mData.logInfo, created);
    return UFALSE;
  }

  UDEBUG("Created render pass {}!", mData.logInfo);
  return UTRUE;
}


b32 FRenderPassVulkan::close(VkDevice device) {
  UTRACE("Closing render pass {}...", mData.logInfo);

  if (mData.renderPass != VK_NULL_HANDLE) {
    UTRACE("Destroying render pass...");
    vkDestroyRenderPass(device, mData.renderPass, nullptr);
    mData.renderPass = VK_NULL_HANDLE;
  }
  else {
    UWARN("As render pass {} is not created, it won't be destroyed!", mData.logInfo);
  }

  mData.logInfo = "";

  UDEBUG("Closed render pass {}!", mData.logInfo);
  return UTRUE;
}


}
