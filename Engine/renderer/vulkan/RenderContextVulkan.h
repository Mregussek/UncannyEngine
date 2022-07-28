
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

  [[nodiscard]] b32 createWindowSurface();
  b32 closeWindowSurface();


  FRenderContextSpecification mSpecs{};

  VkInstance mVkInstance{ VK_NULL_HANDLE };
  VkDebugUtilsMessengerEXT mVkDebugUtilsMsg{ VK_NULL_HANDLE };
  VkPhysicalDevice mVkPhysicalDevice{ VK_NULL_HANDLE };
  VkPhysicalDeviceProperties mVkPhysicalDeviceProperties{};
  VkPhysicalDeviceFeatures mVkPhysicalDeviceFeatures{};
  std::vector<u32> mQueueFamilyIndexVector{};
  std::vector<VkQueueFamilyProperties> mVkQueueFamilyPropertiesVector{};
  VkSurfaceKHR mVkWindowSurface{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
