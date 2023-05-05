
#ifndef UNCANNYENGINE_SWAPCHAIN_H
#define UNCANNYENGINE_SWAPCHAIN_H


#include <volk.h>
#include <vector>
#include <span>
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

  [[nodiscard]] b8 IsRecreatePossible() const;
  void Recreate();

  void WaitForNextImage();
  void Present();

  [[nodiscard]] b8 IsOutOfDate() const { return m_OutOfDate; }
  [[nodiscard]] std::span<const VkImage> GetImages() const { return m_Images; }
  [[nodiscard]] u32 GetBackBufferCount() const { return m_BackBufferCount; }
  [[nodiscard]] u32 GetCurrentFrameIndex() const { return m_CurrentFrame; }
  [[nodiscard]] VkExtent2D GetCurrentExtent() const { return m_CurrentExtent; }
  [[nodiscard]] VkFormat GetFormat() const { return m_Format; }

  [[nodiscard]] const FFence& GetFence() const { return m_Fences[m_CurrentFrame]; }
  [[nodiscard]] const FSemaphore& GetImageAvailableSemaphore() const
  {
    return m_ImageAvailableSemaphores[m_CurrentFrame];
  }
  [[nodiscard]] const FSemaphore& GetPresentableImageReadySemaphore() const
  {
    return m_PresentableImagesReadySemaphores[m_CurrentFrame];
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
  VkExtent2D m_CurrentExtent{};
  VkFormat m_Format{ VK_FORMAT_UNDEFINED };
  u32 m_BackBufferCount{ 0 };
  u32 m_CurrentFrame{ 0 };
  u32 m_ImageIndex{ 0 };
  b8 m_OutOfDate{ UFALSE };

};


}


#endif //UNCANNYENGINE_SWAPCHAIN_H
