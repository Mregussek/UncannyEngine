
#include "ContextVulkan.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


static b32 validateQueuesVectorIsProper(const std::vector<VkQueue>& queueVector,
                                        const char* logInfo);


b32 FRenderContextVulkan::createGraphicsQueues() {
  UTRACE("Creating graphics queues for rendering...");

  // Graphics queues...
  FQueueFamilyDependencies graphicsFamilyDeps{};
  b32 foundGraphicsDeps{
      getQueueFamilyDependencies(EQueueFamilyMainUsage::GRAPHICS,
                                 mPhysicalDeviceDependencies.queueFamilyDependencies,
                                 &graphicsFamilyDeps) };
  if (not foundGraphicsDeps) {
    UERROR("Could not find graphics queue family dependencies!");
    return UFALSE;
  }

  u32 requiredGraphicsQueuesCount{ graphicsFamilyDeps.queuesCountNeeded };
  mVkGraphicsQueueVector.resize(requiredGraphicsQueuesCount);

  for (u32 j = 0; j < requiredGraphicsQueuesCount; j++) {
    vkGetDeviceQueue(mVkDevice, mGraphicsQueueFamilyIndex, j, &mVkGraphicsQueueVector[j]);
  }

  if (not validateQueuesVectorIsProper(mVkGraphicsQueueVector, "graphics")) {
    UERROR("Graphics queues were not retrieved correctly!");
    return UFALSE;
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


b32 FRenderContextVulkan::createTransferQueues() {
  UTRACE("Creating transfer queues for transfer operations...");

  // Transfer queues...
  FQueueFamilyDependencies transferFamilyDeps{};
  b32 foundTransferDeps{
      getQueueFamilyDependencies(EQueueFamilyMainUsage::TRANSFER,
                                 mPhysicalDeviceDependencies.queueFamilyDependencies,
                                 &transferFamilyDeps) };
  if (not foundTransferDeps) {
    UERROR("Could not find transfer queue family dependencies!");
    return UFALSE;
  }

  u32 requiredTransferQueuesCount{ transferFamilyDeps.queuesCountNeeded };
  mVkTransferQueueVector.resize(requiredTransferQueuesCount);

  for (u32 j = 0; j < requiredTransferQueuesCount; j++) {
    vkGetDeviceQueue(mVkDevice, mTransferQueueFamilyIndex, j, &mVkTransferQueueVector[j]);
  }

  if (not validateQueuesVectorIsProper(mVkTransferQueueVector, "transfer")) {
    UERROR("Graphics queues were not retrieved correctly!");
    return UFALSE;
  }

  UTRACE("Assigning copy queue index to 0");
  mCopyQueueIndex = 0;

  UDEBUG("Created transfer queues!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeGraphicsQueues() {
  UTRACE("Closing graphics queues...");

  // Queues are created with logical device, so there is not destroy function
  mVkGraphicsQueueVector.clear();

  UDEBUG("Closed graphics queues!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeTransferQueues() {
  UTRACE("Closing transfer queues...");

  // Queues are created with logical device, so there is not destroy function
  mVkTransferQueueVector.clear();

  UDEBUG("Closed transfer queues!");
  return UTRUE;
}


b32 validateQueuesVectorIsProper(const std::vector<VkQueue>& queueVector, const char* logInfo) {
  if (queueVector.empty()) {
    UERROR("There is no {} queues, vector is empty!", logInfo);
    return UFALSE;
  }

  for (const VkQueue& queue : queueVector) {
    if (queue == VK_NULL_HANDLE) {
      UERROR("There is {} queue, that is null handle!", logInfo);
      return UFALSE;
    }
  }

  UTRACE("{} queue vector is correct!", logInfo);
  return UTRUE;
}


}
