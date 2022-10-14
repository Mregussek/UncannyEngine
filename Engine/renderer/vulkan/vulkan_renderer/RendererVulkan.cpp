
#include "RendererVulkan.h"
#include "VulkanRecordCommandBuffers.h"
#include <renderer/vulkan/vulkan_framework/Utilities.h>
#include <renderer/vulkan/vulkan_renderer/graphics_pipelines/ShaderModulesVulkan.h>
#include <utilities/Logger.h>
#include <vulkan/vulkan_win32.h>


namespace uncanny
{


void FRendererVulkan::defineDependencies() {
  UTRACE("Defining renderer dependencies...");

  // SWAPCHAIN
  mSwapchainDependencies.usedImageCount = 2;
  mSwapchainDependencies.formatCandidatesVector = {
      { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
  };
  mSwapchainDependencies.imageUsageVector = {
      VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  };
  mSwapchainDependencies.imageFormatFeatureVector = {
      VK_FORMAT_FEATURE_TRANSFER_DST_BIT, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
  };

  // RENDER TARGET
  mImageDependencies.renderTarget.formatCandidatesVector = {
      { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
  };
  mImageDependencies.renderTarget.usageVector = {
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  };
  mImageDependencies.renderTarget.formatsFeatureVector = {
      VK_FORMAT_FEATURE_TRANSFER_SRC_BIT, VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
      VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
  };

  // DEPTH
  // Prefer using 24-bit depth formats for optimal performance.
  // Prefer using packed depth/stencil formats. This is a common cause for notable
  // performance differences between an OpenGL and Vulkan implementation.
  // Don’t use 32-bit floating point depth formats, due to the performance cost, unless
  // improved precision is actually required.
  mImageDependencies.depth.formatCandidatesVector = {
      { VK_FORMAT_D32_SFLOAT, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
  };
  mImageDependencies.depth.usageVector = {
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
  };
  mImageDependencies.depth.formatsFeatureVector = {
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  };
}


void FRendererVulkan::getRequiredExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Adding swapchain extension to required...");
  pRequiredExtensions->push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


b32 FRendererVulkan::init(const FRendererSpecification& specs) {
  UTRACE("Initializing vulkan renderer...");

  vkf::FInstanceInitDependenciesVulkan instanceInitDeps{};
  instanceInitDeps.expectedVulkanApiVersion = VK_API_VERSION_1_3;
  instanceInitDeps.pWindow = specs.pWindow;
  instanceInitDeps.appName = specs.appName;
  instanceInitDeps.engineVersion = specs.engineVersion;
  instanceInitDeps.appVersion = specs.appVersion;

  b32 instanceInitialized{ m_Instance.init(instanceInitDeps) };
  if (not instanceInitialized) {
    UFATAL("Cannot run vulkan renderer! Instance failed to initialize!");
    return UFALSE;
  }

#if ENABLE_DEBUGGING_RENDERER
  vkf::FDebugUtilsInitDependenciesVulkan debugUtilsInitDependencies{};
  debugUtilsInitDependencies.instance = m_Instance.Handle();

  b32 debugUtilsInitialized{ m_DebugUtils.init(debugUtilsInitDependencies) };
  if (not debugUtilsInitialized) {
    UFATAL("Could not start debugging renderer! Debug Utils failed to initialize");
    return UFALSE;
  }
#endif

  vkf::FPhysicalDeviceInitDependenciesVulkan physicalDeviceInitDeps{};
  physicalDeviceInitDeps.instance = m_Instance.Handle();

  b32 physicalDeviceInitialized{ m_PhysicalDevice.init(physicalDeviceInitDeps) };
  if (not physicalDeviceInitialized) {
    UFATAL("Cannot run vulkan renderer! Physical device failed to initialize!");
    return UFALSE;
  }

  vkf::FLogicalDeviceInitDependenciesVulkan logicalDeviceInitDeps{};
  logicalDeviceInitDeps.physicalDevice = m_PhysicalDevice.Handle();
  logicalDeviceInitDeps.queueFamilyPropertiesGraphics = m_PhysicalDevice.QueueFamilyPropertiesGraphics();
  logicalDeviceInitDeps.queueFamilyPropertiesTransfer = m_PhysicalDevice.QueueFamilyPropertiesTransfer();
  logicalDeviceInitDeps.queueFamilyIndexGraphics = m_PhysicalDevice.QueueFamilyGraphics();
  logicalDeviceInitDeps.queueFamilyIndexTransfer = m_PhysicalDevice.QueueFamilyTransfer();

  b32 logicalDeviceInitialized{ m_LogicalDevice.init(logicalDeviceInitDeps) };
  if (not logicalDeviceInitialized) {
    UFATAL("Cannot run vulkan renderer! Logical device failed to initialize!");
    return UFALSE;
  }

  vkf::FQueuesInitDependenciesVulkan queuesInitDeps{};
  queuesInitDeps.logicalDevice = m_LogicalDevice.Handle();
  queuesInitDeps.queueFamilyIndexGraphics = m_PhysicalDevice.QueueFamilyGraphics();
  queuesInitDeps.availableQueuesGraphics = m_PhysicalDevice.QueueFamilyPropertiesGraphics().queueCount;
  queuesInitDeps.queueFamilyIndexTransfer = m_PhysicalDevice.QueueFamilyTransfer();
  queuesInitDeps.availableQueuesTransfer = m_PhysicalDevice.QueueFamilyPropertiesTransfer().queueCount;

  b32 queuesInitialized{ m_Queues.init(queuesInitDeps) };
  if (not queuesInitialized) {
    UFATAL("Cannot run vulkan renderer! Queues failed to initialize!");
    return UFALSE;
  }

  vkf::FWindowSurfaceInitDependenciesVulkan windowSurfaceInitDeps{};
  windowSurfaceInitDeps.instance = m_Instance.Handle();
  windowSurfaceInitDeps.physicalDevice = m_PhysicalDevice.Handle();
  windowSurfaceInitDeps.pWindow = specs.pWindow;
  windowSurfaceInitDeps.queueFamilyIndexGraphics = m_PhysicalDevice.QueueFamilyGraphics();

  b32 windowSurfaceInitialized{ m_WindowSurface.init(windowSurfaceInitDeps) };
  if (not windowSurfaceInitialized) {
    UFATAL("Cannot run vulkan renderer! Window Surface failed to initialize!");
    return UFALSE;
  }

  // define all dependencies for vulkan renderer before setup
  defineDependencies();
  b32 properDependenciesDefined{ validateDependencies() };
  if (not properDependenciesDefined) {
    UFATAL("Wrong dependencies defined for vulkan renderer!");
    return UFALSE;
  }

  // assign members
  mMaxFramesInFlight = mSwapchainDependencies.usedImageCount;

  // we can create swapchain for acquiring presentable images
  b32 properlyCreatedSwapchain{ createSwapchain() };
  if (not properlyCreatedSwapchain) {
    UFATAL("Could not create swapchain, cannot acquire images nor present them!");
    return UFALSE;
  }

  // Create graphics pipelines
  b32 properlyCreatedGraphicsPipelines{ createGraphicsPipelinesGeneral() };
  if (not properlyCreatedGraphicsPipelines) {
    UFATAL("Could not create graphics pipelines!");
    return UFALSE;
  }

  // enable depth buffer with depth images for swapchain
  b32 properlyCreatedDepthImage{ createDepthImage() };
  if (not properlyCreatedDepthImage) {
    UFATAL("Could not create depth image, cannot render 3D!");
    return UFALSE;
  }

  // create render target images
  b32 properlyCreatedRenderTargetImages{ createRenderTargetImages() };
  if (not properlyCreatedRenderTargetImages) {
    UFATAL("Could not create render target images, cannot render 3D!");
    return UFALSE;
  }

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

  UINFO("Initialized vulkan renderer!");
  return UTRUE;
}


void FRendererVulkan::terminate() {
  UTRACE("Terminating vulkan renderer...");

  if (m_LogicalDevice.Handle() != VK_NULL_HANDLE) {
    m_LogicalDevice.waitIdle();
  }

  closeGraphicsFences();
  closeGraphicsSemaphores();
  closeCommandBuffers();
  closeCommandPool();
  closeDepthImage();
  closeRenderTargetImages();
  closeGraphicsPipelinesGeneral();
  closeSwapchain();

  UINFO("Terminated vulkan renderer!");
}


b32 FRendererVulkan::parseSceneForRendering(const FRenderSceneConfiguration& sceneConfig) {
  UTRACE("Parsing scene for rendering...");

  mSceneConfig = sceneConfig;

  b32 createdVertexIndexBuffer{ createVertexIndexBuffersForMesh(
      mSceneConfig.pMesh, &mVertexBuffer, &mIndexBuffer) };
  if (not createdVertexIndexBuffer) {
    UERROR("Could not create vertex index buffers for mesh object!");
    return UFALSE;
  }

  FShaderModuleUniformVulkan shaderUniform{};
  FShaderModulesVulkan::fillShaderUniform(mSceneConfig.pCamera, mSceneConfig.pMesh, &shaderUniform);

  b32 createdUniformBuffers{ createUniformBuffer(&shaderUniform, &mUniformBuffer) };
  if (not createdUniformBuffers) {
    UERROR("Could not create uniform buffers!");
    return UFALSE;
  }

  FShaderWriteIntoDescriptorSetDependenciesVulkan writeIntoDescriptorSetDeps{};
  writeIntoDescriptorSetDeps.device = m_LogicalDevice.Handle();
  writeIntoDescriptorSetDeps.pUniformBuffer = &mUniformBuffer;

  mGraphicsPipeline.writeDataIntoDescriptorSet(writeIntoDescriptorSetDeps);

  UINFO("Parsed scene for rendering!");
  return UTRUE;
}


b32 FRendererVulkan::updateSceneDuringRendering(const FRenderSceneConfiguration& sceneConfig) {
  UTRACE("Updating scene during rendering...");
  mSceneConfig = sceneConfig;

  FShaderModuleUniformVulkan shaderUniform{};
  FShaderModulesVulkan::fillShaderUniform(mSceneConfig.pCamera, mSceneConfig.pMesh, &shaderUniform);

  FBufferCopyDependenciesVulkan copyDeps{};
  copyDeps.pNext = nullptr;
  copyDeps.device = m_LogicalDevice.Handle();
  copyDeps.size = sizeof(FShaderModuleUniformVulkan);
  copyDeps.pData = &shaderUniform;

  b32 copied{ mUniformBuffer.copy(copyDeps) };
  if (not copied) {
    UERROR("Could not copy render scene cfg to uniform buffer!");
    return UFALSE;
  }

  UTRACE("Updated scene during rendering!");
  return UTRUE;
}


b32 FRendererVulkan::closeScene() {
  UTRACE("Closing render scene...");
  if (m_LogicalDevice.Handle() != VK_NULL_HANDLE) {
    m_LogicalDevice.waitIdle();
  }

  closeUniformBuffer(&mUniformBuffer);
  closeVertexIndexBuffersForMesh(&mVertexBuffer, &mIndexBuffer);

  UINFO("Closed render scene!");
  return UTRUE;
}


b32 FRendererVulkan::prepareStateForRendering() {
  UTRACE("Preparing state for rendering...");

  // Record commands as startup point
  b32 recordedCommandBuffers{ recordCommandBuffersGeneral() };
  if (not recordedCommandBuffers) {
    UFATAL("Could not record command buffers!");
    return UFALSE;
  }
  resetRenderLoopMembers();

  UINFO("Prepared state for rendering!");
  return UTRUE;
}


void FRendererVulkan::resetRenderLoopMembers() {
  UTRACE("Resetting render loop members...");
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


b32 FRendererVulkan::recordCommandBuffersGeneral() {
  UTRACE("Recording command buffers...");

  b32 properlyResetCommandPoolsAndBuffers{ resetCommandPool(mVkGraphicsCommandPool) };
  if (not properlyResetCommandPoolsAndBuffers) {
    UERROR("Could not reset command pools (with command buffers), so cannot record commands!");
    return UFALSE;
  }

  FGraphicsPipelineRecordCommandsDependencies recordDeps{};
  recordDeps.pRenderTargets = &mImageRenderTargetVector;
  recordDeps.pVertexBuffer = &mVertexBuffer;
  recordDeps.pIndexBuffer = &mIndexBuffer;
  recordDeps.pCommandBuffers = &mVkRenderCommandBufferVector;

  b32 recordPipeline{ mGraphicsPipeline.recordUsageCommands(recordDeps) };
  if (not recordPipeline) {
    UFATAL("Could not record graphics pipeline with vertex buffer for render target cmd buffers!");
    return UFALSE;
  }

  b32 recordedCopyImage{ recordCopyRenderTargetIntoPresentableImage(
      mImageRenderTargetVector, mImagePresentableVector, mVkCopyCommandBufferVector) };
  if (not recordedCopyImage) {
    UFATAL("Could not record copy image command buffers!");
    return UFALSE;
  }

  UTRACE("Recorded command buffers!");
  return UTRUE;
}


ERendererState FRendererVulkan::prepareFrame(const FRendererPrepareFrameSpecification& specs) {
  // if window is minimized, if so we don't want to schedule anything and recreate swapchain
  b32 surfaceImageState{ m_WindowSurface.isMinimized(m_PhysicalDevice.Handle()) };
  if (shouldReturnAfterWindowSurfacePresentableImageStateValidation(surfaceImageState)) {
    return ERendererState::SURFACE_MINIMIZED;
  }

  vkWaitForFences(m_LogicalDevice.Handle(), 1, &mVkFencesInFlightFrames[mCurrentFrame], VK_TRUE,
                  UINT64_MAX);
  vkResetFences(m_LogicalDevice.Handle(), 1, &mVkFencesInFlightFrames[mCurrentFrame]);

  return ERendererState::RENDERING;
}


b32 FRendererVulkan::submitFrame() {
  VkSubmitInfo renderSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  renderSubmitInfo.pNext = nullptr;
  renderSubmitInfo.waitSemaphoreCount = 0;
  renderSubmitInfo.pWaitSemaphores = nullptr;
  renderSubmitInfo.pWaitDstStageMask = nullptr;
  renderSubmitInfo.commandBufferCount = 1;
  renderSubmitInfo.pCommandBuffers = &mVkRenderCommandBufferVector[mCurrentFrame];
  renderSubmitInfo.signalSemaphoreCount = 1;
  renderSubmitInfo.pSignalSemaphores = &mVkSemaphoreRenderingFinishedVector[mCurrentFrame];

  vkf::AssertResultVulkan( vkQueueSubmit(m_Queues.QueueRendering(), 1, &renderSubmitInfo, VK_NULL_HANDLE) );

  VkResult properlyAcquiredNextImage =
      vkAcquireNextImageKHR(m_LogicalDevice.Handle(), mVkSwapchainCurrent, UINT64_MAX,
                            mVkSemaphoreImageAvailableVector[mCurrentFrame],
                            VK_NULL_HANDLE, &mImagePresentableIndex);
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

  VkSemaphore waitCopySemaphores[]{
      mVkSemaphoreRenderingFinishedVector[mCurrentFrame],
      mVkSemaphoreImageAvailableVector[mCurrentFrame] };
  VkPipelineStageFlags copyStageMasks[]{
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT };

  VkSubmitInfo copySubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  copySubmitInfo.pNext = nullptr;
  copySubmitInfo.waitSemaphoreCount = 2;
  copySubmitInfo.pWaitSemaphores = waitCopySemaphores;
  copySubmitInfo.pWaitDstStageMask = copyStageMasks;
  copySubmitInfo.commandBufferCount = 1;
  copySubmitInfo.pCommandBuffers = &mVkCopyCommandBufferVector[mCurrentFrame];
  copySubmitInfo.signalSemaphoreCount = 1;
  copySubmitInfo.pSignalSemaphores = &mVkSemaphoreCopyImageFinishedVector[mCurrentFrame];

  vkf::AssertResultVulkan( vkQueueSubmit(m_Queues.QueueTransfer(), 1, &copySubmitInfo,
                                         mVkFencesInFlightFrames[mCurrentFrame]) );

  VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  queuePresentInfo.pNext = nullptr;
  queuePresentInfo.waitSemaphoreCount = 1;
  queuePresentInfo.pWaitSemaphores = &mVkSemaphoreCopyImageFinishedVector[mCurrentFrame];
  queuePresentInfo.swapchainCount = 1;
  queuePresentInfo.pSwapchains = &mVkSwapchainCurrent;
  queuePresentInfo.pImageIndices = &mImagePresentableIndex;
  queuePresentInfo.pResults = nullptr;

  VkResult properlyPresentedImage{
      vkQueuePresentKHR(m_Queues.QueuePresentation(), &queuePresentInfo) };
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
  return UTRUE;
}


b32 FRendererVulkan::endFrame(const FRendererEndFrameSpecification& specs) {
  // Check if surface is out of date and surface is not minimized
  if (mSurfaceIsOutOfDate and m_WindowSurface.isMinimized(m_PhysicalDevice.Handle())) {
    UTRACE("As surface is out of date and is not minimized, need to recreate swapchain...");

    m_LogicalDevice.waitIdle();

    b32 properlyRecreatedSwapchain{ recreateSwapchain() };
    if (not properlyRecreatedSwapchain) {
      UERROR("Could not recreate swapchain!");
      return UFALSE;
    }

    b32 properlyRecreatedDepthImage{ recreateDepthImage() };
    if (not properlyRecreatedDepthImage) {
      UERROR("Could not recreate depth image!");
      return UFALSE;
    }

    b32 properlyRecreatedRenderTargetImages{ recreateRenderTargetImages() };
    if (not properlyRecreatedRenderTargetImages) {
      UERROR("Could not recreate render target!");
      return UFALSE;
    }

    u32 saveImageIndex{ mImagePresentableIndex };
    b32 preparedUpdatedFrame{ prepareStateForRendering() };
    if (not preparedUpdatedFrame) {
      UERROR("Could not prepare new frame with recreated swapchain!");
      return UFALSE;
    }

    UINFO("Swapchain is recreated, command buffers again recorded, surface should be optimal!"
          " imageIndex: {}, currentFrame: {}", saveImageIndex, mCurrentFrame);
  }

  return UTRUE;
}


b32 FRendererVulkan::shouldReturnAfterWindowSurfacePresentableImageStateValidation(b32 state) {
  if (not state) {
    if (mPrintNotProperExtent) {
      UDEBUG("Window Surface Extent is not proper, probably minimized, skipping update()!");
      mPrintNotProperExtent = UFALSE;
    }
    mPrintCorrectExtent = UTRUE;
    // if window is minimized we should skip update(), so we should return from it
    return UTRUE;
  }
  else {
    if (mPrintCorrectExtent) {
      UDEBUG("Window Surface Extent is correct, resuming update()!");
      mPrintCorrectExtent = UFALSE;
    }
    mPrintNotProperExtent = UTRUE;
    // if otherwise, we shall skip returning and continue update() method.
    return UFALSE;
  }
}


}
