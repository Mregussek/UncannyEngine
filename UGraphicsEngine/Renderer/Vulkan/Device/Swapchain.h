
#ifndef UNCANNYENGINE_SWAPCHAIN_H
#define UNCANNYENGINE_SWAPCHAIN_H


#include <volk.h>
#include <vector>
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Fence.h"
#include "UGraphicsEngine/Renderer/Vulkan/Synchronization/Semaphore.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FLogicalDevice;
class FWindowSurface;


class FSwapchain
{
public:

  void Create(u32 backBufferCount, VkDevice vkDevice, const FWindowSurface* pWindowSurface);
  void Destroy();

  void Recreate();

  void WaitForNextImage();

private:

  void CreateOnlySwapchain(VkSwapchainKHR oldSwapchain);


  std::vector<FFence> m_Fences{};
  std::vector<FSemaphore> m_ImageAvailableSemaphores{};
  std::vector<FSemaphore> m_PresentableImagesReadySemaphores{};
  VkSwapchainKHR m_Swapchain{ VK_NULL_HANDLE };
  VkDevice m_Device{ nullptr };
  const FWindowSurface* m_pWindowSurface{ nullptr };
  u32 m_BackBufferCount{ 0 };

};


}


#endif //UNCANNYENGINE_SWAPCHAIN_H
