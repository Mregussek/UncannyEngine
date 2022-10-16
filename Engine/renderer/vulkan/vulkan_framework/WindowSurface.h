
#ifndef UNCANNYENGINE_WINDOWSURFACE_H
#define UNCANNYENGINE_WINDOWSURFACE_H


#include <vector>
#include <utilities/Variables.h>
#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR 1
#else
#define VK_USE_PLATFORM_WIN32_KHR 0
#endif
#include <volk.h>


namespace uncanny
{
  class FWindow;
}
namespace uncanny::vkf
{


struct FWindowSurfaceInitDependenciesVulkan {
  VkInstance instance{ VK_NULL_HANDLE };
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  const ::uncanny::FWindow* pWindow{ nullptr };
  u32 queueFamilyIndexGraphics{ UUNUSED };
};


class FWindowSurfaceVulkan {
public:

  b32 init(const FWindowSurfaceInitDependenciesVulkan& deps);
  void terminate(VkInstance instance);

  void updateCapabilities(VkPhysicalDevice physicalDevice);

  b32 detectSupportedImageFormat(VkPhysicalDevice physicalDevice,
                                 const std::vector<VkSurfaceFormatKHR>& candidates,
                                 VkSurfaceFormatKHR* pOutFormat);

  [[nodiscard]] b32 isMinimized(VkPhysicalDevice physicalDevice);

  [[nodiscard]] VkSurfaceKHR Handle() const { return m_VkSurface; }
  [[nodiscard]] VkSurfaceCapabilitiesKHR Capabilities() const { return m_VkSurfaceCaps; }
  [[nodiscard]] VkExtent2D Extent() const { return m_VkExtent2D; }
  [[nodiscard]] VkPresentModeKHR PresentMode() const { return m_VkPresentMode; }

private:

  const ::uncanny::FWindow* m_pWindow{ nullptr };
  VkSurfaceKHR m_VkSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR m_VkSurfaceCaps{};
  VkExtent2D m_VkExtent2D{ 0, 0 };
  VkPresentModeKHR m_VkPresentMode{ VK_PRESENT_MODE_FIFO_KHR };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
