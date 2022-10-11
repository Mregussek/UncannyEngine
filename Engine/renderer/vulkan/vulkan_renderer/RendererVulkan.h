
#ifndef UNCANNYENGINE_RENDERERVULKAN_H
#define UNCANNYENGINE_RENDERERVULKAN_H


#include <renderer/Renderer.h>
#include <renderer/vulkan/vulkan_framework/Instance.h>
#include <renderer/vulkan/vulkan_framework/DebugUtils.h>
#include <renderer/vulkan/vulkan_framework/PhysicalDevice.h>
#include <renderer/vulkan/vulkan_framework/LogicalDevice.h>
#include <renderer/vulkan/vulkan_framework/Queues.h>
#include <renderer/vulkan/vulkan_framework/WindowSurface.h>
#include <renderer/vulkan/vulkan_resources/BufferVulkan.h>
#include <renderer/vulkan/vulkan_resources/ImageVulkan.h>
#include "graphics_pipelines/GraphicsPipelinesVulkan.h"
#include "RendererDependenciesVulkan.h"
#include <vector>


namespace uncanny
{


class FWindow;


class FRendererVulkan : public FRenderer {
public:

  b32 init(const FRendererSpecification& specs) override;
  void terminate() override;

  b32 parseSceneForRendering(const FRenderSceneConfiguration& sceneConfig) override;
  b32 updateSceneDuringRendering(const FRenderSceneConfiguration& sceneConfig) override;
  b32 closeScene() override;

  [[nodiscard]] b32 prepareStateForRendering() override;
  [[nodiscard]] ERendererState prepareFrame() override;
  b32 submitFrame() override;
  b32 endFrame() override;

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

  [[nodiscard]] b32 createUniformBuffer(FShaderModuleUniformVulkan* pShaderUniform,
                                        FBufferVulkan* pBuffer);
  b32 closeUniformBuffer(FBufferVulkan* pBuffer);

  [[nodiscard]] b32 recordCommandBuffersGeneral();

  [[nodiscard]] b32 shouldReturnAfterWindowSurfacePresentableImageStateValidation(b32 state);
  void resetRenderLoopMembers();


  // Class Members
  // Vulkan Framework Members
  vkf::FInstanceVulkan m_Instance{};
#if ENABLE_DEBUGGING_RENDERER
  vkf::FDebugUtilsVulkan m_DebugUtils{};
#endif
  vkf::FPhysicalDeviceVulkan m_PhysicalDevice{};
  vkf::FLogicalDeviceVulkan m_LogicalDevice{};
  vkf::FQueuesVulkan m_Queues{};
  vkf::FWindowSurfaceVulkan m_WindowSurface{};
  // Configs, Deps
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
  FBufferVulkan mUniformBuffer{};
  // Render Loop info
  b32 mSurfaceIsOutOfDate{ UFALSE };
  b32 mPrintNotProperExtent{ UFALSE };
  b32 mPrintCorrectExtent{ UFALSE };
  u32 mImagePresentableIndex{ UUNUSED };

};


}


#endif //UNCANNYENGINE_RENDERERVULKAN_H
