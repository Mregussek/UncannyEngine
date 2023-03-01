
#include "WindowSurface.h"
#include "UTools/Window/IWindow.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan {


void FWindowSurface::Create(const ::uncanny::IWindow* pWindow, VkInstance vkInstance) {
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


}
