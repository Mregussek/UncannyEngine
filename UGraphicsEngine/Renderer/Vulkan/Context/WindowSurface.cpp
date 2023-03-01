
#include "WindowSurface.h"
#include "UTools/Window/IWindow.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan {


void FWindowSurface::Create(const ::uncanny::IWindow* pWindow, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice) {
  m_PhysicalDevice = vkPhysicalDevice;

#if defined(WIN32)
  auto vkCreateWin32SurfaceKHR =
      (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(vkInstance, "vkCreateWin32SurfaceKHR");
  VkWin32SurfaceCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.pNext = nullptr;
  createInfo.hinstance = nullptr;
  createInfo.hwnd = pWindow->GetWin32Handle();
  VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &m_Surface);
  AssertVkAndThrow(result);
#else
  #error platform not supported
#endif
}


void FWindowSurface::Destroy(VkInstance vkInstance) {
  if (m_Surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(vkInstance, m_Surface, nullptr);
  }
}


VkSurfaceCapabilitiesKHR FWindowSurface::GetCapabilities() const {
  VkSurfaceCapabilitiesKHR capabilities{};
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &capabilities);
  AssertVkAndThrow(result);
  return capabilities;
}


std::vector<VkSurfaceFormatKHR> FWindowSurface::GetFormats() const {
  u32 count{ 0 };
  vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &count, nullptr);
  std::vector<VkSurfaceFormatKHR> formats(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &count, formats.data());
  return formats;
}


VkFormatProperties FWindowSurface::GetFormatProperties(VkFormat format) const {
  VkFormatProperties properties{};
  vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);
  return properties;
}


std::vector<VkPresentModeKHR> FWindowSurface::GetPresentModes() const {
  u32 count{ 0 };
  vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &count, nullptr);
  std::vector<VkPresentModeKHR> modes(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &count, modes.data());
  return modes;
}


b8 FWindowSurface::IsPresentationSupported(FQueueFamilyIndex queueFamilyIndex) const {
  VkBool32 result{ VK_FALSE };
  vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, queueFamilyIndex, m_Surface, &result);
  return result ? UTRUE : UFALSE;
}


}
