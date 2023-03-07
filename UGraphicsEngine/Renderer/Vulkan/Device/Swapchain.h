
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
class FQueue;


class FSwapchain
{
public:

  void Create(u32 backBufferCount, VkDevice vkDevice, const FQueue* pPresentQueue,
              const FWindowSurface* pWindowSurface);
  void Destroy();

  void Recreate();

  void WaitForNextImage();
  void Present();

  [[nodiscard]] b8 IsOutOfDate() const { return m_OutOfDate; }
  [[nodiscard]] const std::vector<VkImage>& GetImages() const { return m_Images; }
  [[nodiscard]] u32 GetBackBufferCount() const { return m_BackBufferCount; }
  [[nodiscard]] u32 GetCurrentFrameIndex() const { return m_CurrentFrame; }

  [[nodiscard]] VkFence GetFence() const
  {
    return m_Fences.at(m_CurrentFrame).GetHandle();
  }

  [[nodiscard]] VkSemaphore GetImageAvailableSemaphore() const
  {
    return m_ImageAvailableSemaphores[m_CurrentFrame].GetHandle();
  }

  [[nodiscard]] VkSemaphore GetPresentableImageReadySemaphore() const
  {
    return m_PresentableImagesReadySemaphores[m_CurrentFrame].GetHandle();
  }

private:

  void CreateOnlySwapchain(VkSwapchainKHR oldSwapchain);


  std::vector<FFence> m_Fences{};
  std::vector<FSemaphore> m_ImageAvailableSemaphores{};
  std::vector<FSemaphore> m_PresentableImagesReadySemaphores{};
  std::vector<VkImage> m_Images{};
  VkSwapchainKHR m_Swapchain{ VK_NULL_HANDLE };
  VkDevice m_Device{ nullptr };
  const FQueue* m_pPresentQueue{ nullptr };
  const FWindowSurface* m_pWindowSurface{ nullptr };
  u32 m_BackBufferCount{ 0 };
  u32 m_CurrentFrame{ 0 };
  u32 m_ImageIndex{ 0 };
  b8 m_OutOfDate{ UFALSE };

};


}


#endif //UNCANNYENGINE_SWAPCHAIN_H
