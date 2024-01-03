
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


/// @brief FSwapchain is a wrapper class for VkSwapchainKHR. Also it encapsulates all the functionalities of swapchain.
class FSwapchain
{
public:

  void Create(u32 backBufferCount, VkDevice vkDevice, const FQueue* pPresentQueue,
              const FWindowSurface* pWindowSurface);
  void CreateImageViews();
  void CreateFramebuffers(VkRenderPass renderPass, VkImageView depthView);

  void Destroy();

  [[nodiscard]] b8 IsRecreatePossible() const;
  void Recreate();

  void WaitForNextImage();
  void Present();

// Getters
public:

  [[nodiscard]] b8 IsOutOfDate() const { return m_OutOfDate; }
  [[nodiscard]] std::span<const VkImage> GetImages() const { return m_Images; }
  [[nodiscard]] std::span<const VkImageView> GetViews() const { return m_ImageViews; }
  [[nodiscard]] std::span<const VkFramebuffer> GetFramebuffers() const { return m_Framebuffers; }
  [[nodiscard]] u32 GetBackBufferCount() const { return m_BackBufferCount; }
  [[nodiscard]] VkFormat GetFormat() const { return m_Format; }

  [[nodiscard]] u32 GetCurrentFrameIndex() const;
  [[nodiscard]] VkExtent2D GetCurrentExtent() const;
  [[nodiscard]] f32 GetCurrentAspectRatio() const;

  [[nodiscard]] const FFence& GetCurrentFence() const;
  [[nodiscard]] const FSemaphore& GetCurrentImageAvailableSemaphore() const;
  [[nodiscard]] const FSemaphore& GetCurrentPresentableImageReadySemaphore() const;

private:

  void CreateOnlySwapchain(VkSwapchainKHR oldSwapchain);
  void RetrieveNewlyCreatedImages();

  void DestroyViews();
  void DestroyFramebuffers();

private:

  std::vector<FFence> m_Fences{};
  std::vector<FSemaphore> m_ImageAvailableSemaphores{};
  std::vector<FSemaphore> m_PresentableImagesReadySemaphores{};
  std::vector<VkImage> m_Images{};
  std::vector<VkImageView> m_ImageViews{};
  std::vector<VkFramebuffer> m_Framebuffers{};
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
