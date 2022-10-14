
#include "RendererVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


static b32 closeFences(VkDevice device, std::vector<VkFence>* pFencesVector, const char* logInfo);


b32 FRendererVulkan::createGraphicsFences() {
  UTRACE("Creating graphics fences...");

  u32 imageCount{ mSwapchainDependencies.usedImageCount };
  mVkFencesInFlightFrames.resize(imageCount);

  VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (u32 i = 0; i < imageCount; i++) {
    U_VK_ASSERT( vkCreateFence(m_LogicalDevice.Handle(), &createInfo, nullptr,
                               &mVkFencesInFlightFrames[i]) );
  }

  UDEBUG("Created graphics fences!");
  return UTRUE;
}


b32 FRendererVulkan::closeGraphicsFences() {
  UTRACE("Closing graphics fences...");

  closeFences(m_LogicalDevice.Handle(), &mVkFencesInFlightFrames, "in flight");

  UDEBUG("Closed graphics fences!");
  return UTRUE;
}


b32 closeFences(VkDevice device, std::vector<VkFence>* pFencesVector, const char* logInfo) {
  if (pFencesVector->empty()) {
    UWARN("{} fences vector is empty, so nothing will be destroyed!", logInfo);
    return UTRUE;
  }

  for (u32 i = 0; i < pFencesVector->size(); i++) {
    if (pFencesVector->at(i) == VK_NULL_HANDLE) {
      UWARN("As {} fence {} is not created, so it is not destroyed!", logInfo, i);
      continue;
    }

    UTRACE("Destroying {} fence {}...", logInfo, i);
    vkDestroyFence(device, pFencesVector->at(i), nullptr);
    pFencesVector->at(i) = VK_NULL_HANDLE;
  }

  pFencesVector->clear();
  return UTRUE;
}


}
