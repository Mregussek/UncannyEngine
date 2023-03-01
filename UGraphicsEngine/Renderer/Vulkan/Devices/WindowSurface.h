
#ifndef UNCANNYENGINE_WINDOWSURFACE_H
#define UNCANNYENGINE_WINDOWSURFACE_H


#include <volk.h>


namespace uncanny::vulkan {


class FWindowSurface {
public:

private:

  VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR m_Capabilities{};
  VkSurfaceFormatKHR m_Format{ VK_FORMAT_UNDEFINED };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
