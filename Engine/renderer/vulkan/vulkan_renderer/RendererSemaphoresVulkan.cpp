
#include "RendererVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


static void closeSemaphores(VkDevice device, std::vector<vkf::FSemaphoreVulkan>* pSemaphoresVector,
                            const char* logInfo);


b32 FRendererVulkan::createGraphicsSemaphores() {
  UTRACE("Creating graphics semaphores...");

  u32 imageCount{ mSwapchainDependencies.usedImageCount };

  mSemaphoreImageAvailableVector.resize(imageCount);
  mSemaphoreRenderingFinishedVector.resize(imageCount);
  mSemaphoreCopyImageFinishedVector.resize(imageCount);

  vkf::FSemaphoreInitDependenciesVulkan semaphoreInitDeps{};
  semaphoreInitDeps.device = m_LogicalDevice.Handle();

  for (u32 i = 0; i < imageCount; i++) {
    mSemaphoreImageAvailableVector[i].init(semaphoreInitDeps);
    mSemaphoreRenderingFinishedVector[i].init(semaphoreInitDeps);
    mSemaphoreCopyImageFinishedVector[i].init(semaphoreInitDeps);
  }

  UDEBUG("Created graphics semaphores!");
  return UTRUE;
}


b32 FRendererVulkan::closeGraphicsSemaphores() {
  UTRACE("Closing graphics semaphores...");

  closeSemaphores(m_LogicalDevice.Handle(), &mSemaphoreImageAvailableVector,
                  "image available");
  closeSemaphores(m_LogicalDevice.Handle(), &mSemaphoreRenderingFinishedVector,
                  "rendering finished");
  closeSemaphores(m_LogicalDevice.Handle(), &mSemaphoreCopyImageFinishedVector,
                  "copy image finished");

  UDEBUG("Closed graphics semaphores!");
  return UTRUE;
}


void closeSemaphores(VkDevice device, std::vector<vkf::FSemaphoreVulkan>* pSemaphoresVector,
                     const char* logInfo) {
  if (pSemaphoresVector->empty()) {
    UWARN("{} semaphores vector is empty, so nothing will be destroyed!", logInfo);
    return;
  }

  for (auto& semaphore : *pSemaphoresVector) {
    semaphore.terminate(device);
  }

  pSemaphoresVector->clear();
}


}
