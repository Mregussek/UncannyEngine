
#ifndef UNCANNYENGINE_CONTEXTVULKAN_H
#define UNCANNYENGINE_CONTEXTVULKAN_H


#include <renderer/Context.h>
#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#else
  #define VK_USE_PLATFORM_WIN32_KHR 0
#endif
#include <volk.h>
#include "VulkanUtilities.h"
#include "ContextVulkanDependencies.h"


namespace uncanny
{


class FRenderContextVulkan : public FRenderContext {
public:

  b32 init(const FRenderContextSpecification& renderContextSpecs) override;
  void terminate() override;

  ERenderLibrary getLibrary() const override;

  [[nodiscard]] b32 collectWindowSurfaceCapabilities();
  [[nodiscard]] b32 isWindowSurfacePresentableImageExtentProper();
  b32 detectSupportedImageFormatByWindowSurface(
      const std::vector<VkSurfaceFormatKHR>& formatCandidates, VkSurfaceFormatKHR* pOutFormat);

  [[nodiscard]] VkPhysicalDevice PhysicalDevice() const { return mVkPhysicalDevice; }
  [[nodiscard]] VkDevice Device() const { return mVkDevice; }
  [[nodiscard]] VkSurfaceKHR Surface() const { return mVkWindowSurface; }
  [[nodiscard]] VkSurfaceCapabilitiesKHR SurfaceCapabilities() const {return mVkSurfaceCapabilities; }
  [[nodiscard]] VkExtent2D SurfaceExtent() const { return mVkSurfaceExtent2D; }
  [[nodiscard]] VkPresentModeKHR SurfacePresentMode() const { return mVkPresentMode; }
  [[nodiscard]] u32 QueueFamilyIndexGraphics() const { return mGraphicsQueueFamilyIndex; }
  [[nodiscard]] u32 QueueFamilyIndexTransfer() const { return mTransferQueueFamilyIndex; }
  [[nodiscard]] VkQueue QueueCopy() const { return mVkTransferQueueVector[mCopyQueueIndex]; }
  [[nodiscard]] VkQueue QueueRendering() const {
    return mVkGraphicsQueueVector[mRenderingQueueIndex]; }
  [[nodiscard]] VkQueue QueuePresentation() const {
    return mVkGraphicsQueueVector[mPresentationQueueIndex]; }

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
  b32 closeWindowSurface();

  [[nodiscard]] b32 createLogicalDevice();
  b32 closeLogicalDevice();

  [[nodiscard]] b32 createGraphicsQueues();
  b32 closeGraphicsQueues();


  // Class members
  FRenderContextSpecification mSpecs{};
  FInstanceDependencies mInstanceDependencies{};
  FPhysicalDeviceDependencies mPhysicalDeviceDependencies{};
  FWindowSurfaceDependencies mWindowSurfaceDependencies{};
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
  // Window Surface
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR mVkSurfaceCapabilities{};
  VkPresentModeKHR mVkPresentMode{ VK_PRESENT_MODE_FIFO_KHR }; // FIFO has to exist as spec says
  VkExtent2D mVkSurfaceExtent2D{}; // used during swapchain creation, make sure to update it
  // Logical Device
  VkDevice mVkDevice{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_CONTEXTVULKAN_H
