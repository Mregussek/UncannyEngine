
#ifndef UNCANNYENGINE_RENDERCONTEXTVULKAN_H
#define UNCANNYENGINE_RENDERCONTEXTVULKAN_H


#include "renderer/RenderContext.h"
#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#else
  #define VK_USE_PLATFORM_WIN32_KHR 0
#endif
#include <volk.h>
#include "RenderContextDependencies.h"
#include "RenderContextStructures.h"


namespace uncanny
{


class FRenderContextVulkan : public FRenderContext {
public:

  b32 init(FRenderContextSpecification renderContextSpecs) override;
  b32 update() override;
  void terminate() override;

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

  [[nodiscard]] b32 recordCommandBuffersGeneral();

  [[nodiscard]] b32 recordCommandBuffersForClearingColorImage(
      const std::vector<FImageVulkan>& renderTargetImages,
      const std::vector<VkCommandBuffer>& commandBuffers) const;

  [[nodiscard]] b32 recordCommandBuffersForCopyRenderTargetIntoPresentableImage(
      const std::vector<FImageVulkan>& renderTargetImages,
      const std::vector<VkImage>& presentableImages,
      const std::vector<VkCommandBuffer>& commandBuffers) const;

  [[nodiscard]] b32 shouldReturnAfterWindowSurfacePresentableImageStateValidation(b32 state);
  void resetRenderLoopMembers();


  // Class members
  FRenderContextSpecification mSpecs{};
  FInstanceDependencies mInstanceDependencies{};
  FPhysicalDeviceDependencies mPhysicalDeviceDependencies{};
  FSwapchainDependencies mSwapchainDependencies{};
  // Instance
  VkInstance mVkInstance{ VK_NULL_HANDLE };
  // Debugger
  VkDebugUtilsMessengerEXT mVkDebugUtilsMsg{ VK_NULL_HANDLE };
  // Physical Device
  VkPhysicalDevice mVkPhysicalDevice{ VK_NULL_HANDLE };
  // Queue Family
  u32 mGraphicsQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
  std::vector<VkQueueFamilyProperties> mVkQueueFamilyProperties{};
  std::vector<VkQueue> mVkGraphicsQueueVector{};
  u32 mRenderingQueueIndex{ UUNUSED };    // this index is not queue family! It is for mVkGraphicsQueueVector
  u32 mPresentationQueueIndex{ UUNUSED }; // this index is not queue family! It is for mVkGraphicsQueueVector
  // Semaphores - synchronizes GPU to GPU execution of commands
  std::vector<VkSemaphore> mVkSemaphoreImageAvailableVector{};
  std::vector<VkSemaphore> mVkSemaphoreRenderingFinishedVector{};
  std::vector<VkSemaphore> mVkSemaphoreCopyImageFinishedVector{};
  // Window Surface
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR mVkSurfaceCapabilities{};
  VkSurfaceFormatKHR mVkSurfaceFormat{};
  VkPresentModeKHR mVkPresentMode{ VK_PRESENT_MODE_FIFO_KHR }; // FIFO has to exist as spec says
  VkExtent2D mVkImageExtent2D{}; // used during swapchain creation, make sure to update it
  // Logical Device
  VkDevice mVkDevice{ VK_NULL_HANDLE };
  // Swapchain
  VkSwapchainKHR mVkSwapchainCurrent{ VK_NULL_HANDLE };
  VkSwapchainKHR mVkSwapchainOld{ VK_NULL_HANDLE };
  std::vector<VkImage> mVkImagePresentableVector{};
  std::vector<VkImageView> mVkImageViewPresentableVector{};
  // Render Images info
  std::vector<FImageVulkan> mRenderTargetImageVector{};
  // Depth Image info
  VkFormat mVkDepthFormat{ VK_FORMAT_UNDEFINED };
  FImageVulkan mDepthImage{};
  // Command Pools
  VkCommandPool mVkGraphicsCommandPool{ VK_NULL_HANDLE };
  // Command Buffers
  std::vector<VkCommandBuffer> mVkRenderCommandBufferVector{};
  std::vector<VkCommandBuffer> mVkCopyCommandBufferVector{};
  // Frames in flight
  std::vector<VkFence> mVkFencesInFlightFrames{};
  u32 mMaxFramesInFlight{ UUNUSED };
  u32 mCurrentFrame{ UUNUSED };
  // Render Loop info
  b32 mSurfaceIsOutOfDate{ UFALSE };
  b32 mPrintNotProperExtent{ UFALSE };
  b32 mPrintCorrectExtent{ UFALSE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
