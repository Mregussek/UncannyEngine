
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
public:

  void Create(const uncanny::IWindow* pWindow, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);
  void Destroy(VkInstance vkInstance);

  void UpdateCapabilities();
  void UpdateFormats();
  void UpdatePresentModes();

  [[nodiscard]] b8 IsPresentationSupported(FQueueFamilyIndex queueFamilyIndex) const;

private:

  std::vector<VkSurfaceFormatKHR> m_SupportedFormats{};
  std::vector<VkPresentModeKHR> m_SupportedPresentModes{};
  VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
  VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR m_Capabilities{};
  VkSurfaceFormatKHR m_Format{ VK_FORMAT_UNDEFINED };
  VkPresentModeKHR m_PresentMode{ VK_PRESENT_MODE_FIFO_KHR };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
