
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
  // My AMD card support only one 1 queue at graphics queue family, my NVIDIA supports 16!
  graphicsQueueFamilyDependencies.queuesCountNeeded = 1;
  graphicsQueueFamilyDependencies.queuesPriorities = { 1.f, 1.f };
  graphicsQueueFamilyDependencies.graphics = UTRUE;

  mPhysicalDeviceDependencies.deviceType = EPhysicalDeviceType::DISCRETE;
  mPhysicalDeviceDependencies.deviceTypeFallback = EPhysicalDeviceType::INTEGRATED;
  mPhysicalDeviceDependencies.queueFamilyIndexesCount = 1;
  mPhysicalDeviceDependencies.queueFamilyDependencies = { graphicsQueueFamilyDependencies };
  // Prefer using 24-bit depth formats for optimal performance.
  // Prefer using packed depth/stencil formats. This is a common cause for notable
  // performance differences between an OpenGL and Vulkan implementation.
  // Don’t use 32-bit floating point depth formats, due to the performance cost, unless
  // improved precision is actually required.
  mPhysicalDeviceDependencies.depthFormatDependencies = {
      VK_FORMAT_D24_UNORM_S8_UINT
  };

  mSwapchainDependencies.usedImageCount = 2;
  mSwapchainDependencies.imageUsageVector = {
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT
  };
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
    UFATAL("Could not create Vulkan Instance! Check if drivers or Vulkan SDK on"
           "this host is properly installed!");
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

  // We create logical device (dependent only on VkPhysicalDevice) knowing that present
  // on newly created window surface is available
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
  // b32 properlyCreatedDepthImages{ createDepthImages() };
  // if (not properlyCreatedDepthImages) {
  //   UFATAL("Could not create depth images, cannot render 3D!");
  //   return UFALSE;
  // }

  // create command pools for command buffers
  b32 properlyCreatedCommandPools{ createCommandPool() };
  if (not properlyCreatedCommandPools) {
    UFATAL("Could not create command pools, cannot send commands to GPU!");
    return UFALSE;
  }
  b32 properlyCreatedCommandBuffers{ createCommandBuffers() };
  if (not properlyCreatedCommandBuffers) {
    UFATAL("Could not create command buffers, cannot send commands to GPU!");
    return UFALSE;
  }

  // Create synchronization semaphores and fences
  b32 properlyCreatedGraphicsSemaphores{ createGraphicsSemaphores() };
  if (not properlyCreatedGraphicsSemaphores) {
    UFATAL("Could not create graphics semaphores, cannot synchronize GPU-GPU operations!");
    return UFALSE;
  }
  b32 properlyCreatedFencesForCommandBuffers{ createGraphicsFences() };
  if (not properlyCreatedFencesForCommandBuffers) {
    UFATAL("Could not create fences, cannot synchronize CPU-GPU operations!");
    return UFALSE;
  }

  // Finally record commands as startup point
  b32 recordedCommandBuffers{
    recordCommandBuffersForClearingColorImage(mVkImagePresentableVector, mVkGraphicsCommandPool,
                                              mVkGraphicsCommandBufferVector) };
  if (not recordedCommandBuffers) {
    UFATAL("Could not record command buffers!");
    return UFALSE;
  }

  mMaxFramesInFlight = mSwapchainDependencies.usedImageCount;
  resetRenderLoopMembers();

  UINFO("Initialized Vulkan Render Context!");
  return UTRUE;
}


void FRenderContextVulkan::resetRenderLoopMembers() {
  // surface is always proper after creation
  mSurfaceIsOutOfDate = UFALSE;
  // set current frame to 0, max value should be mSwapchainDependencies.usedImageCount
  mCurrentFrame = 0;
  // by default, we want to print first occurrence of wrong surface extent
  mPrintNotProperExtent = UTRUE;
  // by default, we don't want to print first occurrence of correct surface extent as after
  // creation it should be always correct
  mPrintCorrectExtent = UFALSE;
}


b32 FRenderContextVulkan::shouldReturnAfterWindowSurfacePresentableImageStateValidation(b32 state) {
  if (not state) {
    if (mPrintNotProperExtent) {
      UDEBUG("Window Surface Extent is not proper, probably minimized, skipping update()!");
      mPrintNotProperExtent = UFALSE;
    }
    mPrintCorrectExtent = UTRUE;
    return UTRUE;
  }
  else {
    if (mPrintCorrectExtent) {
      UDEBUG("Window Surface Extent is correct, resuming update()!");
      mPrintCorrectExtent = UFALSE;
    }
    mPrintNotProperExtent = UTRUE;
    return UFALSE;
  }
}


void FRenderContextVulkan::terminate() {
  UTRACE("Terminating Vulkan Render Context...");
  if (mVkDevice != VK_NULL_HANDLE) {
    UTRACE("Waiting for Device Idle state before terminating...");
    vkDeviceWaitIdle(mVkDevice);
  }

  closeGraphicsFences();
  closeGraphicsSemaphores();
  closeCommandBuffers();
  closeCommandPool();
  //closeDepthImages();
  closeSwapchain();
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
  // if window is minimized, if so we don't want to schedule anything and recreate swapchain
  b32 surfaceImageState{ isWindowSurfacePresentableImageExtentProper() };
  if (shouldReturnAfterWindowSurfacePresentableImageStateValidation(surfaceImageState)) {
    return UTRUE;
  }

  vkWaitForFences(mVkDevice, 1, &mVkFencesInFlightFrames[mCurrentFrame], VK_TRUE, UINT64_MAX);
  vkResetFences(mVkDevice, 1, &mVkFencesInFlightFrames[mCurrentFrame]);

  u32 imageIndex{ UUNUSED };
  VkResult properlyAcquiredNextImage =
      vkAcquireNextImageKHR(mVkDevice, mVkSwapchainCurrent, UINT64_MAX,
                            mVkSemaphoreImageAvailableVector[mCurrentFrame],
                            VK_NULL_HANDLE, &imageIndex);
  switch(properlyAcquiredNextImage) {
    case VK_SUCCESS: break;
    case VK_SUBOPTIMAL_KHR:
    case VK_ERROR_OUT_OF_DATE_KHR: {
      mSurfaceIsOutOfDate = UTRUE;
      break;
    }
    default: {
      UERROR("Other error than expected during acquiring next image!");
      return UFALSE;
    }
  }

  VkPipelineStageFlags waitDstStageMask{ VK_PIPELINE_STAGE_TRANSFER_BIT };

  VkSubmitInfo queueSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  queueSubmitInfo.pNext = nullptr;
  queueSubmitInfo.waitSemaphoreCount = 1;
  queueSubmitInfo.pWaitSemaphores = &mVkSemaphoreImageAvailableVector[mCurrentFrame];
  queueSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  queueSubmitInfo.commandBufferCount = 1;
  queueSubmitInfo.pCommandBuffers = &mVkGraphicsCommandBufferVector[mCurrentFrame];
  queueSubmitInfo.signalSemaphoreCount = 1;
  queueSubmitInfo.pSignalSemaphores = &mVkSemaphoreRenderingFinishedVector[mCurrentFrame];

  U_VK_ASSERT( vkQueueSubmit(mVkGraphicsQueueVector[mRenderingQueueIndex], 1, &queueSubmitInfo,
                             mVkFencesInFlightFrames[mCurrentFrame]) );

  VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  queuePresentInfo.pNext = nullptr;
  queuePresentInfo.waitSemaphoreCount = 1;
  queuePresentInfo.pWaitSemaphores = &mVkSemaphoreRenderingFinishedVector[mCurrentFrame];
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
      mSurfaceIsOutOfDate = UTRUE;
      break;
    }
    default: {
      UERROR("Other error than expected during acquiring next image!");
      return UFALSE;
    }
  }

  mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;

  // Check if surface is out of date and surface is not minimized
  if (mSurfaceIsOutOfDate and isWindowSurfacePresentableImageExtentProper()) {
    UTRACE("As surface is out of date and is not minimized, need to recreate swapchain...");

    vkDeviceWaitIdle(mVkDevice);

    b32 properlyRecreatedSwapchain{ recreateSwapchain() };
    if (not properlyRecreatedSwapchain) {
      UERROR("Could not recreate swapchain after acquiring next image!");
      return UFALSE;
    }

    b32 recordedCommandBuffers{
        recordCommandBuffersForClearingColorImage(mVkImagePresentableVector, mVkGraphicsCommandPool,
                                                  mVkGraphicsCommandBufferVector) };
    if (not recordedCommandBuffers) {
      UERROR("Could not record command buffers!");
      return UFALSE;
    }

    UDEBUG("Swapchain is recreated, command buffers again recorded, surface should be optimal!"
           " imageIndex: {}, currentFrame: {}", imageIndex, mCurrentFrame);
    resetRenderLoopMembers();
  }
  return UTRUE;
}


}
