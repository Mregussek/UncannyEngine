
#include "RendererVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


static void closeFences(VkDevice device, std::vector<vkf::FFenceVulkan>* pFencesVector,
                        const char* logInfo);


b32 FRendererVulkan::createGraphicsFences() {
  UTRACE("Creating graphics fences...");

  u32 imageCount{ mSwapchainDependencies.usedImageCount };
  mFencesInFlightFrames.resize(imageCount);

  vkf::FFenceInitDependenciesVulkan fenceInitDeps{};
  fenceInitDeps.device = m_LogicalDevice.Handle();

  for(auto& fence : mFencesInFlightFrames) {
    fence.init(fenceInitDeps);
  }

  UDEBUG("Created graphics fences!");
  return UTRUE;
}


b32 FRendererVulkan::closeGraphicsFences() {
  UTRACE("Closing graphics fences...");

  closeFences(m_LogicalDevice.Handle(), &mFencesInFlightFrames, "in flight");

  UDEBUG("Closed graphics fences!");
  return UTRUE;
}


void closeFences(VkDevice device, std::vector<vkf::FFenceVulkan>* pFencesVector,
                const char* logInfo) {
  if (pFencesVector->empty()) {
    UWARN("{} fences vector is empty, so nothing will be destroyed!", logInfo);
    return;
  }

  for (auto& fence : *pFencesVector) {
    fence.terminate(device);
  }

  pFencesVector->clear();
}


}
