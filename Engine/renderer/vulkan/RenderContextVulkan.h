
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
  [[nodiscard]] b32 closeInstance();

  [[nodiscard]] b32 createDebugUtilsMessenger();
  [[nodiscard]] b32 closeDebugUtilsMessenger();

  [[nodiscard]] b32 createPhysicalDevice();

  [[nodiscard]] b32 createWindowSurface();


  FRenderContextSpecification mSpecs{};

  VkInstance mInstanceVk{ VK_NULL_HANDLE };
  VkDebugUtilsMessengerEXT mDebugUtilsMsg{ VK_NULL_HANDLE };
  VkPhysicalDevice mPhysicalDeviceVk{ VK_NULL_HANDLE };
  VkPhysicalDeviceProperties mPhysicalDeviceProperties{};
  VkPhysicalDeviceFeatures mPhysicalDeviceFeatures{};
  u32 mQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
  VkQueueFamilyProperties mQueueFamilyProperties{};

};


}


#endif //UNCANNYENGINE_RENDERCONTEXTVULKAN_H
