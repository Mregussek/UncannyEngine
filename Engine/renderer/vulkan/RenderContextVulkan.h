
#ifndef UNCANNYENGINE_RENDERCONTEXTVULKAN_H
#define UNCANNYENGINE_RENDERCONTEXTVULKAN_H


#include "renderer/RenderContext.h"
#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#else
  #define VK_USE_PLATFORM_WIN32_KHR 0
#endif
#include <volk.h>


namespace uncanny
{


enum class EQueueFamilyMainUsage {
  NONE, GRAPHICS
};


enum class EPhysicalDeviceType {
  NONE, INTEGRATED, DISCRETE
};


struct FSwapchainDependencies {
  // @brief count of images used
  u32 usedImageCount{ UUNUSED };
};


struct FQueueFamilyDependencies {
  // @brief queues priorities for given count
  std::vector<f32> queuesPriorities{};
  // @brief main usage for queue family
  EQueueFamilyMainUsage mainUsage{ EQueueFamilyMainUsage::NONE };
  // @brief how many queues of such type are needed
  u32 queuesCountNeeded{ 0 };
  // @brief support graphics operations
  u32 graphics{ UFALSE };
  // @brief support compute operations
  u32 compute{ UFALSE };
  // @brief support transfer operations
  u32 transfer{ UFALSE };
  // @brief support sparse memory management operations
  u32 sparseBinding{ UFALSE };
};


struct FPhysicalDeviceDependencies {
  // @brief what type of device do you prefer -> integrated / discrete
  EPhysicalDeviceType deviceType{ EPhysicalDeviceType::NONE };
  // @brief if above is not available, what fallback device is preferred
  EPhysicalDeviceType deviceTypeFallback{ EPhysicalDeviceType::NONE };
  // @brief how many queue family indexes types are needed (for graphics, for transfer etc.)
  u32 queueFamilyIndexesCount{ 0 };
  // @brief dependencies for queue families. Count should be equal to queueFamilyIndexesCount
  std::vector<FQueueFamilyDependencies> queueFamilyDependencies{};
  // @brief dependencies for depth format
  std::vector<VkFormat> depthFormatDependencies{};
};


struct FInstanceDependencies {
  // @brief vulkan api version expected
  u32 vulkanApiVersion{ 0 };
};


class FRenderContextVulkan : public FRenderContext {
public:

  b32 init(FRenderContextSpecification renderContextSpecs) override;
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
  b32 closeWindowSurface();

  [[nodiscard]] b32 createLogicalDevice();
  b32 closeLogicalDevice();

  [[nodiscard]] b32 createGraphicsQueues();
  b32 closeGraphicsQueues();

  [[nodiscard]] b32 createGraphicsSemaphores();
  b32 closeGraphicsSemaphores();

  [[nodiscard]] b32 createSwapchain();
  b32 closeSwapchain();

  [[nodiscard]] b32 createDepthImages();
  b32 closeDepthImages();

  [[nodiscard]] b32 createCommandPool();
  b32 closeCommandPool();

  [[nodiscard]] b32 createCommandBuffer();
  b32 closeCommandBuffer();


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
  VkSemaphore mVkSemaphoreImageAvailable{ VK_NULL_HANDLE };
  VkSemaphore mVkSemaphoreRenderingFinished{ VK_NULL_HANDLE };
  // Window Surface
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR mVkSurfaceCapabilities{};
  VkSurfaceFormatKHR mVkSurfaceFormat{};
  VkPresentModeKHR mVkPresentMode{ VK_PRESENT_MODE_FIFO_KHR }; // FIFO has to exist as spec says
  VkExtent2D mVkImageExtent2D{};
  // Logical Device
  VkDevice mVkDevice{ VK_NULL_HANDLE };
  // Swapchain
  VkSwapchainKHR mVkSwapchainCurrent{ VK_NULL_HANDLE };
  VkSwapchainKHR mVkSwapchainOld{ VK_NULL_HANDLE };
  std::vector<VkImage> mVkImagePresentableVector{};
  std::vector<VkImageView> mVkImagePresentableViewVector{};
  // Depth info
  VkFormat mVkDepthFormat{ VK_FORMAT_UNDEFINED };
  VkImage mVkDepthImage{ VK_NULL_HANDLE };
  VkImageView mVkDepthImageView{ VK_NULL_HANDLE };
  VkDeviceMemory mVkDepthImageMemory{ VK_NULL_HANDLE };
  // Command Pools
  VkCommandPool mVkGraphicsCommandPool{ VK_NULL_HANDLE };
  // Command Buffers
  VkCommandBuffer mVkGraphicsCommandBuffer{ VK_NULL_HANDLE };

};


// @brief Implementation at VulkanPhysicalDevice.cpp
FQueueFamilyDependencies getQueueFamilyDependencies(
    EQueueFamilyMainUsage mainUsage,
    const std::vector<FQueueFamilyDependencies>& queueFamilyDependenciesVector);


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
