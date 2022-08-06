
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRenderContextVulkan::createGraphicsQueues() {
  UTRACE("Creating graphics queues for rendering...");

  // Make sure that length match queue indexes and properties vectors
  if (mQueueFamilyIndexVector.size() != mVkQueueFamilyPropertiesVector.size()) {
    UERROR("Length of queues family index vector does not match properties vector size!");
    return UFALSE;
  }

  // Iterate over all queue families and retrieve queue from logical device if possible
  for (u32 i = 0; i < mQueueFamilyIndexVector.size(); i++) {
    UTRACE("Iterating over queue family index {}, making sure that queue has graphics support...",
           mQueueFamilyIndexVector[i]);

    if (not(mVkQueueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      UTRACE("{} queue family index does not support graphics, continuing loop...",
             mQueueFamilyIndexVector[i]);
      continue;
    }

    u32 graphicsQueuesNeeded{
        mPhysicalDeviceDependencies.queueFamilyDependencies[i].queuesCountNeeded };

    if (graphicsQueuesNeeded > mVkQueueFamilyPropertiesVector[i].queueCount) {
      UWARN("{} queue family index does not have enough queues, skipping...");
      continue;
    }

    UTRACE("Retrieving {} graphics queues for family index {} to member variable... ",
           graphicsQueuesNeeded, mQueueFamilyIndexVector[i]);
    for (u32 j = 0; j < graphicsQueuesNeeded; j++) {
      VkQueue& graphicsQueue{ mVkGraphicsQueueVector.emplace_back(VK_NULL_HANDLE) };
      vkGetDeviceQueue(mVkDevice, mQueueFamilyIndexVector[i], j, &graphicsQueue);
    }
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
