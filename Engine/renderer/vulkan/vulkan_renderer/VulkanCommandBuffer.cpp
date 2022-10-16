
#include "RendererVulkan.h"
#include <renderer/vulkan/vulkan_framework/Utilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


static b32 freeCommandBufferVector(
    VkDevice device, VkCommandPool commandPool,
    std::vector<VkCommandBuffer>* pCommandBufferVector, const char* logInfo);


b32 FRendererVulkan::createCommandBuffers() {
  UTRACE("Creating command buffers...");

  mVkRenderCommandBufferVector.resize(mSwapchainDependencies.usedImageCount);
  mVkCopyCommandBufferVector.resize(mSwapchainDependencies.usedImageCount);

  VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = mVkGraphicsCommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = mSwapchainDependencies.usedImageCount;

  vkf::AssertResultVulkan( vkAllocateCommandBuffers(m_LogicalDevice.Handle(), &allocateInfo,
                                                    mVkRenderCommandBufferVector.data()) );

  allocateInfo.commandPool = mVkTransferCommandPool;

  vkf::AssertResultVulkan( vkAllocateCommandBuffers(m_LogicalDevice.Handle(), &allocateInfo,
                                                    mVkCopyCommandBufferVector.data()) );

  UDEBUG("Created command buffers!");
  return UTRUE;
}


b32 FRendererVulkan::closeCommandBuffers() {
  UTRACE("Closing command buffers...");

  freeCommandBufferVector(m_LogicalDevice.Handle(), mVkGraphicsCommandPool,
                          &mVkRenderCommandBufferVector, "render");
  freeCommandBufferVector(m_LogicalDevice.Handle(), mVkTransferCommandPool,
                          &mVkCopyCommandBufferVector, "copy");

  UDEBUG("Closed command buffers!");
  return UTRUE;
}


b32 freeCommandBufferVector(VkDevice device, VkCommandPool commandPool,
                            std::vector<VkCommandBuffer>* pCommandBufferVector,
                            const char* logInfo) {
  if (pCommandBufferVector->empty()) {
    UWARN("Not freeing any {} command buffers as vector is empty!", logInfo);
    return UTRUE;
  }

  for (u32 i = 0; i < pCommandBufferVector->size(); i++) {
    if (pCommandBufferVector->at(i) != VK_NULL_HANDLE) {
      UTRACE("Freeing {} command buffer {} ...", logInfo, i);
      vkFreeCommandBuffers(device, commandPool, 1, &pCommandBufferVector->at(i));
    }
    else {
      UWARN("Not freeing {} command buffer {} as it wasn't been created!", logInfo, i);
    }
  }
  pCommandBufferVector->clear();
  return UTRUE;
}


}
