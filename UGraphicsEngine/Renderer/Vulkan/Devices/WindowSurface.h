
#ifndef UNCANNYENGINE_WINDOWSURFACE_H
#define UNCANNYENGINE_WINDOWSURFACE_H


#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include <volk.h>


namespace uncanny {
class IWindow;
}
namespace uncanny::vulkan {


class FWindowSurface {
public:

  void Create(const uncanny::IWindow* pWindow, VkInstance vkInstance);
  void Destroy(VkInstance vkInstance);

private:

  VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR m_Capabilities{};
  VkSurfaceFormatKHR m_Format{ VK_FORMAT_UNDEFINED };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
