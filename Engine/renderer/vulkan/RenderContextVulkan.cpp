
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FRenderContextVulkan::defineDependencies() {
  UTRACE("Defining dependencies...");

  mInstanceDependencies.vulkanApiVersion = VK_API_VERSION_1_3;

  FQueueFamilyDependencies graphicsQueueFamilyDependencies{};
  graphicsQueueFamilyDependencies.mainUsage = EQueueFamilyMainUsage::GRAPHICS;
  graphicsQueueFamilyDependencies.queuesCountNeeded = 2;
  graphicsQueueFamilyDependencies.queuesPriorities = { 1.f, 1.f };
  graphicsQueueFamilyDependencies.graphics = UTRUE;

  mPhysicalDeviceDependencies.deviceType = EPhysicalDeviceType::DISCRETE;
  mPhysicalDeviceDependencies.deviceTypeFallback = EPhysicalDeviceType::INTEGRATED;
  mPhysicalDeviceDependencies.queueFamilyIndexesCount = 1;
  mPhysicalDeviceDependencies.queueFamilyDependencies = { graphicsQueueFamilyDependencies };
  mPhysicalDeviceDependencies.depthFormatDependencies = {
      VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
  };

  mSwapchainDependencies.usedImageCount = 2;
}


b32 FRenderContextVulkan::init(FRenderContextSpecification renderContextSpecs) {
  UTRACE("Initializing Vulkan Render Context...");

  // assign specs
  mSpecs = renderContextSpecs;

  // define all dependencies for vulkan renderer before setup
  defineDependencies();
  b32 properDependenciesDefined{ validateDependencies() };
  if (not properDependenciesDefined) {
    UFATAL("Wrong dependencies defined for vulkan renderer!");
    return UFALSE;
  }

  // Firstly creating instance as it is mandatory for proper Vulkan work...
  b32 properlyCreatedInstance{ createInstance() };
  if (not properlyCreatedInstance) {
    UFATAL("Could not create Vulkan Instance!");
    return UFALSE;
  }

  // Additionally if flag is enabled create vulkan debugger
  if constexpr (U_VK_DEBUG) {
    b32 properlyCreatedDebugUtilsMsg{ createDebugUtilsMessenger() };
    if (not properlyCreatedDebugUtilsMsg) {
      UERROR("Could not create Debug Utils Messenger!");
      return UFALSE;
    }
  }

  // Secondly creating physical device (needed for logical device)...
  b32 properlyCreatedPhysicalDevice{ createPhysicalDevice() };
  if (not properlyCreatedPhysicalDevice) {
    UFATAL("Could not pick Vulkan Physical Device!");
    return UFALSE;
  }

  // We can create window surface (dependent only on VkInstance) and check presentation
  // support with physical device that is why third step is window surface creation...
  b32 properlyCreatedWindowSurface{ createWindowSurface() };
  if (not properlyCreatedWindowSurface) {
    UFATAL("Could not create window surface, rendering is not available!");
    return UFALSE;
  }

  // We create logical device (dependent only on VkPhysicalDevice)
  b32 properlyCreatedLogicalDevice{ createLogicalDevice() };
  if (not properlyCreatedLogicalDevice) {
    UFATAL("Could not create logical device!");
    return UFALSE;
  }

  // Assigning all graphics command queues for rendering operations
  b32 properlyCreatedGraphicsQueues{ createGraphicsQueues() };
  if (not properlyCreatedGraphicsQueues) {
    UFATAL("There is no graphics queues, rendering in not available!");
    return UFALSE;
  }
  b32 properlyCreatedGraphicsSemaphores{ createGraphicsSemaphores() };
  if (not properlyCreatedGraphicsSemaphores) {
    UFATAL("Could not create graphics semaphores, cannot synchronize device!");
    return UFALSE;
  }

  // we can create swapchain for acquiring presentable images
  b32 properSwapchainDependencies{ areSwapchainDependenciesCorrect() };
  if (not properSwapchainDependencies) {
    UFATAL("Wrong swapchain dependencies passed for creation, cannot acquire images nor present!");
    return UFALSE;
  }
  b32 properlyCreatedSwapchain{ createSwapchain() };
  if (not properlyCreatedSwapchain) {
    UFATAL("Could not create swapchain, cannot acquire images nor present them!");
    return UFALSE;
  }

  // enable depth buffer with depth images for swapchain
  b32 properlyCreatedDepthImages{ createDepthImages() };
  if (not properlyCreatedDepthImages) {
    UFATAL("Could not create depth images, cannot render 3D!");
    return UFALSE;
  }

  // create command pools for command buffers
  b32 properlyCreatedCommandPools{ createCommandPool() };
  if (not properlyCreatedCommandPools) {
    UFATAL("Could not create command pools, cannot send commands to GPU!");
    return UFALSE;
  }
  b32 properlyCreatedCommandBuffers{ createCommandBuffer() };
  if (not properlyCreatedCommandBuffers) {
    UFATAL("Could not create command buffers, cannot send commands to GPU!");
    return UFALSE;
  }

  UINFO("Initialized Vulkan Render Context!");
  return UTRUE;
}


void FRenderContextVulkan::terminate() {
  UTRACE("Terminating Vulkan Render Context...");

  closeCommandBuffer();
  closeCommandPool();
  closeDepthImages();
  closeSwapchain();
  closeGraphicsSemaphores();
  closeGraphicsQueues();
  closeLogicalDevice();
  closeWindowSurface();
  closePhysicalDevice();
  if constexpr (U_VK_DEBUG) {
    closeDebugUtilsMessenger();
  }
  closeInstance();

  UINFO("Terminated Vulkan Render Context!");
}


b32 FRenderContextVulkan::update() {
  u32 imageAcquireTimeout{ UINT32_MAX };
  u32 imageIndex{ UUNUSED };
  VkResult properlyAcquiredNextImage = vkAcquireNextImageKHR(mVkDevice,
                                                             mVkSwapchainCurrent,
                                                             imageAcquireTimeout,
                                                             mVkSemaphoreImageAvailable,
                                                             VK_NULL_HANDLE,
                                                             &imageIndex);
  switch(properlyAcquiredNextImage) {
    case VK_SUCCESS: break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {
      b32 properlyRecreatedSwapchain{ recreateSwapchain() };
      if (not properlyRecreatedSwapchain) {
        UERROR("Could not recreate swapchain after acquiring next image!");
        return UFALSE;
      }
      break;
    }
    default: {
      UERROR("Other error than expected during acquiring next image!");
      return UFALSE;
    }
  }

  VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  commandBufferBeginInfo.pNext = nullptr;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  commandBufferBeginInfo.pInheritanceInfo = nullptr;

  VkClearColorValue clearColor = {
      { 1.0f, 0.8f, 0.4f, 0.0f }
  };

  VkImageSubresourceRange imageSubresourceRange{};
  imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresourceRange.baseMipLevel = 0;
  imageSubresourceRange.levelCount = 1;
  imageSubresourceRange.baseArrayLayer = 0;
  imageSubresourceRange.layerCount = 1;

  u32 memoryBarrierCount{ 0 };
  const VkMemoryBarrier* pMemoryBarriers{ nullptr };
  u32 bufferMemoryBarrierCount{ 0 };
  const VkBufferMemoryBarrier* pBufferMemoryBarriers{ nullptr };
  u32 imageMemoryBarrierCount{ 0 };
  const VkImageMemoryBarrier* pImageMemoryBarriers{ nullptr };

  for (u32 i = 0; i < mVkImagePresentableVector.size(); i++) {
    VkImageMemoryBarrier barrierFromPresentToClear{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrierFromPresentToClear.pNext = nullptr;
    barrierFromPresentToClear.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrierFromPresentToClear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrierFromPresentToClear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierFromPresentToClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierFromPresentToClear.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromPresentToClear.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromPresentToClear.image = mVkImagePresentableVector[i];
    barrierFromPresentToClear.subresourceRange = imageSubresourceRange;

    VkImageMemoryBarrier barrierFromClearToPresent{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrierFromPresentToClear.pNext = nullptr;
    barrierFromPresentToClear.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrierFromPresentToClear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrierFromPresentToClear.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierFromPresentToClear.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierFromPresentToClear.srcQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromPresentToClear.dstQueueFamilyIndex = mGraphicsQueueFamilyIndex;
    barrierFromPresentToClear.image = mVkImagePresentableVector[i];
    barrierFromPresentToClear.subresourceRange = imageSubresourceRange;

    vkBeginCommandBuffer(mVkGraphicsCommandBufferVector[i], &commandBufferBeginInfo);

    memoryBarrierCount = 0;
    pMemoryBarriers = nullptr;
    bufferMemoryBarrierCount = 0;
    pBufferMemoryBarriers = nullptr;
    imageMemoryBarrierCount = 1;
    pImageMemoryBarriers = &barrierFromPresentToClear;

    vkCmdPipelineBarrier(mVkGraphicsCommandBufferVector[i],
                         VkPipelineStageFlags{ VK_PIPELINE_STAGE_TRANSFER_BIT },
                         VkPipelineStageFlags{ VK_PIPELINE_STAGE_TRANSFER_BIT },
                         VkDependencyFlags{ 0 },
                         memoryBarrierCount, pMemoryBarriers,
                         bufferMemoryBarrierCount, pBufferMemoryBarriers,
                         imageMemoryBarrierCount, pImageMemoryBarriers);

    u32 rangeCount{1};
    const VkImageSubresourceRange* pRanges{ &imageSubresourceRange };

    vkCmdClearColorImage(mVkGraphicsCommandBufferVector[i],
                         mVkImagePresentableVector[i],
                         VkImageLayout{ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
                         &clearColor,
                         rangeCount, pRanges);

    memoryBarrierCount = 0;
    pMemoryBarriers = nullptr;
    bufferMemoryBarrierCount = 0;
    pBufferMemoryBarriers = nullptr;
    imageMemoryBarrierCount = 1;
    pImageMemoryBarriers = &barrierFromClearToPresent;

    vkCmdPipelineBarrier(mVkGraphicsCommandBufferVector[i],
                         VkPipelineStageFlags{ VK_PIPELINE_STAGE_TRANSFER_BIT },
                         VkPipelineStageFlags{ VK_PIPELINE_STAGE_TRANSFER_BIT },
                         VkDependencyFlags{ 0 },
                         memoryBarrierCount, pMemoryBarriers,
                         bufferMemoryBarrierCount, pBufferMemoryBarriers,
                         imageMemoryBarrierCount, pImageMemoryBarriers);

    VkResult properlyRecordedCommands{ vkEndCommandBuffer(mVkGraphicsCommandBufferVector[i]) };

    if (properlyRecordedCommands != VK_SUCCESS) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }
  }

  VkPipelineStageFlags waitDstStageMask{ VK_PIPELINE_STAGE_TRANSFER_BIT };

  VkSubmitInfo queueSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  queueSubmitInfo.pNext = nullptr;
  queueSubmitInfo.waitSemaphoreCount = 1;
  queueSubmitInfo.pWaitSemaphores = &mVkSemaphoreImageAvailable;
  queueSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  queueSubmitInfo.commandBufferCount = 1;
  queueSubmitInfo.pCommandBuffers;
  queueSubmitInfo.signalSemaphoreCount = 1;
  queueSubmitInfo.pSignalSemaphores = &mVkSemaphoreRenderingFinished;

  VkFence noFence{ VK_NULL_HANDLE };
  U_VK_ASSERT( vkQueueSubmit(mVkGraphicsQueueVector[mPresentationQueueIndex], 1, &queueSubmitInfo,
                             noFence) );

  VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  queuePresentInfo.pNext = nullptr;
  queuePresentInfo.waitSemaphoreCount = 1;
  queuePresentInfo.pWaitSemaphores = &mVkSemaphoreRenderingFinished;
  queuePresentInfo.swapchainCount = 1;
  queuePresentInfo.pSwapchains = &mVkSwapchainCurrent;
  queuePresentInfo.pImageIndices = &imageIndex;
  queuePresentInfo.pResults = nullptr;

  VkResult properlyPresentedImage{ vkQueuePresentKHR(mVkGraphicsQueueVector[mPresentationQueueIndex],
                                                     &queuePresentInfo) };
  switch(properlyPresentedImage) {
    case VK_SUCCESS: break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {
      b32 properlyRecreatedSwapchain{ recreateSwapchain() };
      if (not properlyRecreatedSwapchain) {
        UERROR("Could not recreate swapchain after presentation!");
        return UFALSE;
      }
      break;
    }
    default: {
      UERROR("Other error than expected during acquiring next image!");
      return UFALSE;
    }
  }

  return UTRUE;
}


b32 FRenderContextVulkan::validateDependencies() const {
  UTRACE("Validating dependencies for vulkan renderer...");
  const auto& physDevDeps{ mPhysicalDeviceDependencies }; // wrapper

  // make sure proper vulkan api version is given
  if (mInstanceDependencies.vulkanApiVersion == 0) {
    UERROR("Wrong Vulkan API version given in dependencies!");
    return UFALSE;
  }

  // make sure there is proper queue family index count
  if (physDevDeps.queueFamilyIndexesCount < 1) {
    UERROR("Queue family indexes count is less than 1, which means no operations on GPU!");
    return UFALSE;
  }

  // make sure device type is possible to select
  if (
      physDevDeps.deviceType == EPhysicalDeviceType::NONE or
      physDevDeps.deviceTypeFallback == EPhysicalDeviceType::NONE) {
    UERROR("Defined device types are NONE, which means proper GPU cannot be selected!");
    return UFALSE;
  }

  // make sure that depth dependencies are correct
  if (physDevDeps.depthFormatDependencies.empty()) {
    UERROR("No depth dependencies info!");
    return UFALSE;
  }

  // make sure that there is not undefined depth format
  for (VkFormat depthFormat : physDevDeps.depthFormatDependencies) {
    if (depthFormat == VK_FORMAT_UNDEFINED) {
      UERROR("One of depth formats is undefined, check it!");
      return UFALSE;
    }
  }

  // make sure there is enough queue dependencies for every queue family
  if (physDevDeps.queueFamilyIndexesCount > physDevDeps.queueFamilyDependencies.size()) {
    UERROR("There is more queue families than dependencies for queue families, not enough info!");
    return UFALSE;
  }

  // make sure that every queue of expected queue family is correctly defined
  for (u32 i = 0; i < physDevDeps.queueFamilyIndexesCount; i++) {
    const auto& queueDeps{ physDevDeps.queueFamilyDependencies[i] }; // wrapper

    // make sure there is proper queue for queue family
    if (queueDeps.queuesCountNeeded < 1) {
      UERROR("Needed queues for family are less than 1, which means no operations on GPU!");
      return UFALSE;
    }

    // make sure that is sth supported for queue
    if (
        (not queueDeps.graphics) && (not queueDeps.compute) &&
        (not queueDeps.transfer) && (not queueDeps.sparseBinding)) {
      UERROR("There is no support info for queue provided, not enough info!");
      return UFALSE;
    }

    // make sure there is enough info about every queue needed for queue family
    if (queueDeps.queuesCountNeeded > queueDeps.queuesPriorities.size()) {
      UERROR("There is more queues need than provided info about them!");
      return UFALSE;
    }

    // make sure every queue has correct info provided
    for (u32 j = 0; j < queueDeps.queuesCountNeeded; j++) {
      // make sure there is proper queue type
      if (queueDeps.mainUsage == EQueueFamilyMainUsage::NONE) {
        UERROR("Queue type for queue family is NONE, wrong info provided!");
        return UFALSE;
      }

      // make sure priority is between range
      if (not (0.f <= queueDeps.queuesPriorities[j] && queueDeps.queuesPriorities[j] <= 1.f)) {
        UERROR("Queue priority is not between range <0.f, 1.f>, wrong info provided!");
        return UFALSE;
      }
    }
  }

  if (mSwapchainDependencies.usedImageCount < 2) {
    UERROR("Minimal image count for swapchain is 2, back and front buffers! Wrong dependencies!");
    return UFALSE;
  }

  UDEBUG("Properly defined dependencies for vulkan renderer!");
  return UTRUE;
}


}
