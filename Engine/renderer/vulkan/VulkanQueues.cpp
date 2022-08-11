
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsQueues() {
  UTRACE("Creating graphics queues for rendering...");

  FQueueFamilyDependencies graphicsFamilyDependencies{
      getQueueFamilyDependencies(EQueueFamilyMainUsage::GRAPHICS,
                                 mPhysicalDeviceDependencies.queueFamilyDependencies) };

  for (u32 j = 0; j < graphicsFamilyDependencies.queuesCountNeeded; j++) {
    VkQueue& graphicsQueue{ mVkGraphicsQueueVector.emplace_back(VK_NULL_HANDLE) };
    vkGetDeviceQueue(mVkDevice, mGraphicsQueueFamilyIndex, j, &graphicsQueue);
  }

  if (mVkGraphicsQueueVector.empty()) {
    UERROR("There is no graphics queues, cannot render anything!");
    return UFALSE;
  }

  for (VkQueue& graphicsQueue : mVkGraphicsQueueVector) {
    if (graphicsQueue == VK_NULL_HANDLE) {
      UERROR("There is graphics queue, that is null handle, cannot render anything!");
      return UFALSE;
    }
  }

  if (mVkGraphicsQueueVector.size() >= 2) {
    UTRACE("Assigning rendering and present queue indexes to 0 and 1 as there is at least 2 queues");
    mRenderingQueueIndex = 0;
    mPresentationQueueIndex = 1;
  }
  else {
    UTRACE("Assigning render and present queue indexes to the same 0 index as there is no 2 queues");
    mRenderingQueueIndex = 0;
    mPresentationQueueIndex = 0;
  }

  UDEBUG("Created graphics queues!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsQueues() {
  UTRACE("Closing graphics queues...");

  // Queues are created with logical device, so there is not destroy function
  mVkGraphicsQueueVector.clear();

  UDEBUG("Closed graphics queues!");
  return UTRUE;
}


}
