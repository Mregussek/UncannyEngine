
#ifndef UNCANNYENGINE_RENDERERVULKAN_H
#define UNCANNYENGINE_RENDERERVULKAN_H


#include <renderer/Renderer.h>
#include <renderer/vulkan/vulkan_resources/BufferVulkan.h>
#include <renderer/vulkan/vulkan_resources/ImageVulkan.h>
#include "graphics_pipelines/GraphicsPipelinesVulkan.h"
#include "RendererDependenciesVulkan.h"
#include <vector>


namespace uncanny
{


class FRenderContextVulkan;


class FRendererVulkan : public FRenderer {
public:

  b32 init() override;
  void terminate() override;

  b32 parseSceneForRendering(const FRenderSceneConfiguration& sceneConfiguration) override;
  b32 closeScene() override;

  [[nodiscard]] b32 prepareStateForRendering() override;
  [[nodiscard]] ERendererState prepareFrame() override;
  b32 submitFrame() override;
  b32 endFrame() override;

  void passContext(FRenderContextVulkan* pContext);
  static void getRequiredExtensions(std::vector<const char*>* pRequiredExtensions);

private:

  void defineDependencies();
  [[nodiscard]] b32 validateDependencies() const;

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

  [[nodiscard]] b32 createGraphicsPipelinesGeneral();
  b32 closeGraphicsPipelinesGeneral();

  [[nodiscard]] b32 createVertexIndexBuffersForMesh(FMesh* pMesh, FBufferVulkan* pOutVertex,
                                                    FBufferVulkan* pOutIndex) const;
  b32 closeVertexIndexBuffersForMesh(FBufferVulkan* pVertex, FBufferVulkan* pIndex) const;

  [[nodiscard]] b32 createUniformBuffers(const FRenderSceneConfiguration& sceneConfiguration);
  b32 closeUniformBuffers();

  [[nodiscard]] b32 recordCommandBuffersGeneral();

  [[nodiscard]] b32 shouldReturnAfterWindowSurfacePresentableImageStateValidation(b32 state);
  void resetRenderLoopMembers();


  // Class Members
  FRenderContextVulkan* mContextPtr{ nullptr };
  FRenderSceneConfiguration mSceneConfig{};
  FSwapchainDependencies mSwapchainDependencies{};
  FImagesDependencies mImageDependencies{};
  // Swapchain
  VkSwapchainKHR mVkSwapchainCurrent{ VK_NULL_HANDLE };
  VkSwapchainKHR mVkSwapchainOld{ VK_NULL_HANDLE };
  std::vector<FImageVulkan> mImagePresentableVector{};
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
  // Semaphores
  std::vector<VkSemaphore> mVkSemaphoreImageAvailableVector{};
  std::vector<VkSemaphore> mVkSemaphoreRenderingFinishedVector{};
  std::vector<VkSemaphore> mVkSemaphoreCopyImageFinishedVector{};
  // Frames in flight
  std::vector<VkFence> mVkFencesInFlightFrames{};
  u32 mMaxFramesInFlight{ UUNUSED };
  u32 mCurrentFrame{ UUNUSED };
  // Pipelines
  FGraphicsPipelineVulkan mGraphicsPipeline{};
  // Buffers
  FBufferVulkan mVertexBuffer{};
  FBufferVulkan mIndexBuffer{};
  FBufferVulkan mUniformBufferCamera{};
  // Render Loop info
  b32 mSurfaceIsOutOfDate{ UFALSE };
  b32 mPrintNotProperExtent{ UFALSE };
  b32 mPrintCorrectExtent{ UFALSE };
  u32 mImagePresentableIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_RENDERERVULKAN_H
