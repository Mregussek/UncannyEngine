
#ifndef UNCANNYENGINE_SWAPCHAIN_H
#define UNCANNYENGINE_SWAPCHAIN_H


#include <volk.h>


namespace uncanny::vulkan {


class FLogicalDevice;
class FWindowSurface;


class FSwapchain {
public:

  void Create(const FLogicalDevice* pLogicalDevice, const FWindowSurface* pWindowSurface);
  void Destroy();

private:

  VkSwapchainKHR m_Swapchain{ VK_NULL_HANDLE };
  VkSwapchainKHR m_OldSwapchain{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SWAPCHAIN_H
