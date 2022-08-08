
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


enum class EQueueFamilyType {
  NONE, GRAPHICS
};


enum class EQueueType {
  NONE, RENDERING, PRESENTING
};


enum class EPhysicalDeviceType {
  NONE, INTEGRATED, DISCRETE
};


struct FQueueFamily {
  u32 index{ ~0u };
  EQueueFamilyType type{ EQueueFamilyType::NONE };
};


struct FQueueFamilyDependencies {
  // @brief queues priorities for given count
  std::vector<f32> queuesPriorities{};
  // @brief queues types for given count (render need to know for what reason is that queue)
  std::vector<EQueueType> queuesTypes{};
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
  // @brief type for queue families. Count should be equal to queueFamilyIndexesCount
  std::vector<EQueueFamilyType> queueFamilyTypes{};
  // @brief dependencies for queue families. Count should be equal to queueFamilyIndexesCount
  std::vector<FQueueFamilyDependencies> queueFamilyDependencies{};
  // @brief dependencies for depth format
  std::vector<VkFormat> depthFormatDependencies{};
};


class FRenderContextVulkan : public FRenderContext {
public:

  b32 init(FRenderContextSpecification renderContextSpecs) override;
  void terminate() override;

private:

  void defineDependencies();
  b32 validateDependencies() const;

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

  [[nodiscard]] b32 createSwapchain();
  b32 closeSwapchain();

  [[nodiscard]] b32 createDepthImages();
  b32 closeDepthImages();


  // Class members
  FRenderContextSpecification mSpecs{};
  FPhysicalDeviceDependencies mPhysicalDeviceDependencies{};
  // Instance
  VkInstance mVkInstance{ VK_NULL_HANDLE };
  // Debugger
  VkDebugUtilsMessengerEXT mVkDebugUtilsMsg{ VK_NULL_HANDLE };
  // Physical Device
  VkPhysicalDevice mVkPhysicalDevice{ VK_NULL_HANDLE };
  VkPhysicalDeviceProperties mVkPhysicalDeviceProperties{};
  VkPhysicalDeviceFeatures mVkPhysicalDeviceFeatures{};
  VkPhysicalDeviceMemoryProperties mVkPhysicalDeviceMemoryProperties{};
  // Queue Family
  std::vector<FQueueFamily> mQueueFamilyVector{};
  std::vector<VkQueueFamilyProperties> mVkQueueFamilyPropertiesVector{};
  u32 mGraphicsQueueFamilyIndex{ UUNUSED }; // must be used as index to mQueueFamilyVector
  // Window Surface
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR mVkSurfaceCapabilities{};
  VkSurfaceFormatKHR mVkSurfaceFormat{};
  VkPresentModeKHR mVkPresentMode{ VK_PRESENT_MODE_FIFO_KHR }; // FIFO has to exist as spec says
  VkExtent2D mVkImageExtent2D{};
  // Logical Device
  VkDevice mVkDevice{ VK_NULL_HANDLE };
  // Queues
  std::vector<VkQueue> mVkGraphicsQueueVector{};
  // Swapchain
  VkSwapchainKHR mVkSwapchainCurrent{ VK_NULL_HANDLE };
  VkSwapchainKHR mVkSwapchainOld{ VK_NULL_HANDLE };
  std::vector<VkImage> mVkImagePresentableVector{};
  std::vector<VkImageView> mVkImagePresentableViewVector{};
  std::vector<VkFramebuffer> mVkFramebufferVector{};
  // Depth info
  VkFormat mVkDepthFormat{ VK_FORMAT_UNDEFINED };
  VkImage mVkDepthImage{ VK_NULL_HANDLE };
  VkImageView mVkDepthImageView{ VK_NULL_HANDLE };
  VkDeviceMemory mVkDepthImageMemory{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
