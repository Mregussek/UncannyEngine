
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
  m_SupportedFormats.clear();
  m_SupportedFormats.shrink_to_fit();
}


void FWindowSurface::UpdateCapabilities() {
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_Capabilities);
  AssertVkAndThrow(result);
}


void FWindowSurface::UpdateFormats() {
  m_SupportedFormats.clear();

  u32 count{ 0 };
  vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &count, nullptr);
  m_SupportedFormats.resize(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &count, m_SupportedFormats.data());
}


void FWindowSurface::UpdatePresentModes() {
  m_SupportedPresentModes.clear();

  u32 count{ 0 };
  vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &count, nullptr);
  m_SupportedPresentModes.resize(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &count, m_SupportedPresentModes.data());
}


b8 FWindowSurface::IsPresentationSupported(FQueueFamilyIndex queueFamilyIndex) const {
  VkBool32 result{ VK_FALSE };
  vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, queueFamilyIndex, m_Surface, &result);
  return result ? UTRUE : UFALSE;
}


}
