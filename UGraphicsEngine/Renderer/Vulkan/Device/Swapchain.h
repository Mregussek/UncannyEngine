
#ifndef UNCANNYENGINE_SWAPCHAIN_H
#define UNCANNYENGINE_SWAPCHAIN_H


#include <volk.h>
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Fence.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Semaphore.h"


namespace uncanny::vulkan {


class FLogicalDevice;
class FWindowSurface;


class FSwapchain {
public:

  void Create(VkDevice vkDevice, const FWindowSurface* pWindowSurface);
  void Destroy();

  void Recreate();

private:

  void CreateOnlySwapchain();


  FFence m_Fence{};
  VkSwapchainKHR m_Swapchain{ VK_NULL_HANDLE };
  VkSwapchainKHR m_OldSwapchain{ VK_NULL_HANDLE };
  VkDevice m_Device{ nullptr };
  const FWindowSurface* m_pWindowSurface{ nullptr };

};


}


#endif //UNCANNYENGINE_SWAPCHAIN_H
