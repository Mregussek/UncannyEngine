
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


class FRenderContextVulkan : public FRenderContext {
public:

  b32 init(FRenderContextSpecification renderContextSpecs) override;
  void terminate() override;

private:

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


  // Class members
  FRenderContextSpecification mSpecs{};
  // Instance
  VkInstance mVkInstance{ VK_NULL_HANDLE };
  // Debugger
  VkDebugUtilsMessengerEXT mVkDebugUtilsMsg{ VK_NULL_HANDLE };
  // Physical Device members
  VkPhysicalDevice mVkPhysicalDevice{ VK_NULL_HANDLE };
  VkPhysicalDeviceProperties mVkPhysicalDeviceProperties{};
  VkPhysicalDeviceFeatures mVkPhysicalDeviceFeatures{};
  VkPhysicalDeviceMemoryProperties mVkPhysicalDeviceMemoryProperties{};
  // QueueFamily Members
  std::vector<u32> mQueueFamilyIndexVector{};
  std::vector<VkQueueFamilyProperties> mVkQueueFamilyPropertiesVector{};
  // Window Surface
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };
  // Logical Device
  VkDevice mVkDevice{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
