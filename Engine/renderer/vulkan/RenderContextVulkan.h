
#ifndef UNCANNYENGINE_RENDERCONTEXTVULKAN_H
#define UNCANNYENGINE_RENDERCONTEXTVULKAN_H


#include "renderer/RenderContext.h"
#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#else
  #define VK_USE_PLATFORM_WIN32_KHR 0
#endif
#include <volk.h>
#include "VulkanUtilities.h"
#include "RenderContextDependencies.h"
#include "RenderContextStructures.h"


namespace uncanny
{


class FRenderContextVulkan : public FRenderContext {
public:

  b32 init(const FRenderContextSpecification& renderContextSpecs) override;
  void terminate() override;

  b32 parseSceneForRendering(const FRenderSceneConfiguration& sceneConfiguration);

  [[nodiscard]] b32 prepareStateForRendering() override;
  [[nodiscard]] ERenderContextState prepareFrame() override;
  b32 submitFrame() override;
  b32 endFrame() override;

private:

  void defineDependencies();
  [[nodiscard]] b32 validateDependencies() const;

  [[nodiscard]] b32 createInstance();
  b32 closeInstance();

  [[nodiscard]] b32 createDebugUtilsMessenger();
  b32 closeDebugUtilsMessenger();

  [[nodiscard]] b32 createPhysicalDevice();
  b32 closePhysicalDevice();

  [[nodiscard]] b32 createWindowSurface();
  [[nodiscard]] b32 collectWindowSurfaceCapabilities();
  [[nodiscard]] b32 isWindowSurfacePresentableImageExtentProper();
  b32 closeWindowSurface();

  [[nodiscard]] b32 createLogicalDevice();
  b32 closeLogicalDevice();

  [[nodiscard]] b32 createGraphicsQueues();
  b32 closeGraphicsQueues();

  [[nodiscard]] b32 areSwapchainDependenciesCorrect();
  [[nodiscard]] b32 createSwapchain();
  [[nodiscard]] b32 recreateSwapchain();
  b32 closeSwapchain();

  [[nodiscard]] b32 createRenderPasses();
  b32 closeRenderPasses();

  [[nodiscard]] b32 createRenderTargetImages();
  [[nodiscard]] b32 recreateRenderTargetImages();
  b32 closeRenderTargetImages();

  [[nodiscard]] b32 createDepthImage();
  [[nodiscard]] b32 recreateDepthImage();
  b32 closeDepthImage();

  [[nodiscard]] b32 createCommandPool();
  b32 resetCommandPool(const VkCommandPool& commandPool);
  b32 closeCommandPool();

  [[nodiscard]] b32 createCommandBuffers();
  b32 closeCommandBuffers();

  [[nodiscard]] b32 createGraphicsSemaphores();
  b32 closeGraphicsSemaphores();

  [[nodiscard]] b32 createGraphicsFences();
  b32 closeGraphicsFences();

  b32 collectViewportScissorInfo();
  [[nodiscard]] b32 createGraphicsPipelinesGeneral();
  b32 closeGraphicsPipelinesGeneral();

  [[nodiscard]] b32 createVertexBuffer(FMesh* pMesh);
  b32 closeVertexBuffer();

  [[nodiscard]] b32 recordCommandBuffersGeneral();

  [[nodiscard]] b32 shouldReturnAfterWindowSurfacePresentableImageStateValidation(b32 state);
  void resetRenderLoopMembers();


  // Class members
  FRenderContextSpecification mSpecs{};
  FInstanceDependencies mInstanceDependencies{};
  FPhysicalDeviceDependencies mPhysicalDeviceDependencies{};
  FWindowSurfaceDependencies mWindowSurfaceDependencies{};
  FSwapchainDependencies mSwapchainDependencies{};
  FImagesDependencies mImageDependencies{};
  // Instance
  VkInstance mVkInstance{ VK_NULL_HANDLE };
  // Debugger
#if U_VK_DEBUG
  VkDebugUtilsMessengerEXT mVkDebugUtilsMsg{ VK_NULL_HANDLE };
#endif
  // Physical Device
  VkPhysicalDevice mVkPhysicalDevice{ VK_NULL_HANDLE };
  // Queue Family
  u32 mGraphicsQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
  u32 mTransferQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
  std::vector<VkQueueFamilyProperties> mVkQueueFamilyProperties{};
  std::vector<VkQueue> mVkGraphicsQueueVector{};
  u32 mRenderingQueueIndex{ UUNUSED }; // Index for mVkGraphicsQueueVector, not queue family
  u32 mPresentationQueueIndex{ UUNUSED }; // Index for mVkGraphicsQueueVector, not queue family
  std::vector<VkQueue> mVkTransferQueueVector{};
  u32 mCopyQueueIndex{ UUNUSED }; // Index for mVkTransferQueueVector, not queue family
  // Semaphores - synchronizes GPU to GPU execution of commands
  std::vector<VkSemaphore> mVkSemaphoreImageAvailableVector{};
  std::vector<VkSemaphore> mVkSemaphoreRenderingFinishedVector{};
  std::vector<VkSemaphore> mVkSemaphoreCopyImageFinishedVector{};
  // Window Surface
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR mVkSurfaceCapabilities{};
  VkPresentModeKHR mVkPresentMode{ VK_PRESENT_MODE_FIFO_KHR }; // FIFO has to exist as spec says
  VkExtent2D mVkSurfaceExtent2D{}; // used during swapchain creation, make sure to update it
  // Logical Device
  VkDevice mVkDevice{ VK_NULL_HANDLE };
  // Swapchain
  VkSwapchainKHR mVkSwapchainCurrent{ VK_NULL_HANDLE };
  VkSwapchainKHR mVkSwapchainOld{ VK_NULL_HANDLE };
  std::vector<FImageVulkan> mImagePresentableVector{};
  // Render Passes
  VkRenderPass mVkRenderPass{ VK_NULL_HANDLE };
  // Render Images info
  std::vector<FImageVulkan> mImageRenderTargetVector{};
  // Depth Image info
  VkFormat mVkDepthFormat{ VK_FORMAT_UNDEFINED };
  FImageVulkan mDepthImage{};
  // Graphics Command Pools
  VkCommandPool mVkGraphicsCommandPool{ VK_NULL_HANDLE };
  std::vector<VkCommandBuffer> mVkRenderCommandBufferVector{};
  // Transfer Command Pools
  VkCommandPool mVkTransferCommandPool{ VK_NULL_HANDLE };
  std::vector<VkCommandBuffer> mVkCopyCommandBufferVector{};
  // Frames in flight
  std::vector<VkFence> mVkFencesInFlightFrames{};
  u32 mMaxFramesInFlight{ UUNUSED };
  u32 mCurrentFrame{ UUNUSED };
  // Pipelines
  VkPipelineLayout mVkPipelineLayoutTriangle{ VK_NULL_HANDLE };
  VkPipeline mVkPipelineTriangle{ VK_NULL_HANDLE };
  VkPipelineLayout mVkPipelineLayoutMeshColor{ VK_NULL_HANDLE };
  VkPipeline mVkPipelineMeshColor{ VK_NULL_HANDLE };
  VkViewport mVkViewport{};
  VkRect2D mVkScissor{};
  // Buffers
  FVertexBufferVulkan mVertexBufferTriangle{};
  // Render Loop info
  b32 mSurfaceIsOutOfDate{ UFALSE };
  b32 mPrintNotProperExtent{ UFALSE };
  b32 mPrintCorrectExtent{ UFALSE };
  u32 mImagePresentableIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
