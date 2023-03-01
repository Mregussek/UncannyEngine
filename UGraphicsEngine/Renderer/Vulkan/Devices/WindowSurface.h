
#ifndef UNCANNYENGINE_WINDOWSURFACE_H
#define UNCANNYENGINE_WINDOWSURFACE_H


#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include <volk.h>
#include "UTools/UTypes.h"
#include "LogicalDeviceAttributes.h"


namespace uncanny {
class IWindow;
}
namespace uncanny::vulkan {


class FWindowSurface {

  // I want RenderContext to access Create() and Destroy() and to check presentation support with IsPresentationSupported()
  friend class FRenderContext;

public:

  [[nodiscard]] VkSurfaceCapabilitiesKHR GetCapabilities() const;
  [[nodiscard]] std::vector<VkSurfaceFormatKHR> GetFormats() const;
  [[nodiscard]] std::vector<VkPresentModeKHR> GetPresentModes() const;

private:

  void Create(const uncanny::IWindow* pWindow, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);
  void Destroy(VkInstance vkInstance);

  [[nodiscard]] b8 IsPresentationSupported(FQueueFamilyIndex queueFamilyIndex) const;

  VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
  VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
