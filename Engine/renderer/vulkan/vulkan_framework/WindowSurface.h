
#ifndef UNCANNYENGINE_WINDOWSURFACE_H
#define UNCANNYENGINE_WINDOWSURFACE_H


#include <volk.h>
#include <vector>
#include <utilities/Variables.h>


namespace uncanny {
  class FWindow;
}
namespace uncanny::vkf {


struct FWindowSurfaceInitDependenciesVulkan {
  VkInstance instance{ VK_NULL_HANDLE };
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  ::uncanny::FWindow* pWindow{ nullptr };
  u32 queueFamilyIndexGraphics{ UUNUSED };
};


class FWindowSurfaceVulkan {
public:

  b32 init(const FWindowSurfaceInitDependenciesVulkan& deps);
  void terminate(VkInstance instance);

  void updateCapabilities(VkPhysicalDevice physicalDevice, const FWindow* pWindow);

  [[nodiscard]] b32 isMinimized(VkPhysicalDevice physicalDevice, const FWindow* pWindow);

  [[nodiscard]] VkSurfaceKHR Handle() const { return m_VkSurface; }
  [[nodiscard]] VkSurfaceCapabilitiesKHR Capabilities() const { return m_VkSurfaceCaps; }
  [[nodiscard]] VkExtent2D Extent() const { return m_VkExtent2D; }
  [[nodiscard]] VkPresentModeKHR PresentMode() const { return m_VkPresentMode; }

private:

  VkSurfaceKHR m_VkSurface{ VK_NULL_HANDLE };
  VkSurfaceCapabilitiesKHR m_VkSurfaceCaps{};
  VkExtent2D m_VkExtent2D{ 0, 0 };
  VkPresentModeKHR m_VkPresentMode{ VK_PRESENT_MODE_FIFO_KHR };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
