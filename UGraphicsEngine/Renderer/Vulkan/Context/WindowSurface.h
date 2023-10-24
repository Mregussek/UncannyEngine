
#ifndef UNCANNYENGINE_WINDOWSURFACE_H
#define UNCANNYENGINE_WINDOWSURFACE_H


#ifdef WIN32
	#define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include <volk.h>
#include "UTools/UTypes.h"
#include "UTools/Window/IWindow.h"
#include "LogicalDeviceAttributes.h"


namespace uncanny::vulkan
{


/// @brief FWindowSurface is a wrapper class for VkSurfaceKHR.
/// @details For the end user FWindowSurface exposes only interface to retrieve data about surface. Those
/// methods firstly query all necessary information and then return it. That is why theirs impl are hidden.
/// @friends As this window surface may travel across several classes in RenderDevice I decided to hide
/// Create() and Destroy() methods from the end user and make friendship with RenderContext so that only
/// RenderContext can manage its lifetime.
class FWindowSurface
{

	// I want RenderContext to access Create() and Destroy() and to check presentation support
	friend class FRenderContext;

public:

	FWindowSurface() = default;

	FWindowSurface(const FWindowSurface& other) = delete;
	FWindowSurface(FWindowSurface&& other) = delete;

	FWindowSurface& operator=(const FWindowSurface& other) = delete;
	FWindowSurface& operator=(FWindowSurface&& other) = delete;

public:

	/// @brief Queries surface capabilities using vkGetPhysicalDeviceSurfaceCapabilitiesKHR
	[[nodiscard]] VkSurfaceCapabilitiesKHR QueryCapabilities() const;

	/// @brief Queries available VKFormats using vkGetPhysicalDeviceSurfaceFormatsKHR
	[[nodiscard]] std::vector<VkSurfaceFormatKHR> QueryFormats() const;

	/// @brief Queries information about provided VkFormat using vkGetPhysicalDeviceFormatProperties
	[[nodiscard]] VkFormatProperties QueryFormatProperties(VkFormat format) const;

	/// @brief Queries available present modes using vkGetPhysicalDeviceSurfacePresentModesKHR
	[[nodiscard]] std::vector<VkPresentModeKHR> QueryPresentModes() const;

	/// @brief Just returns VkSurfaceKHR handle
	[[nodiscard]] VkSurfaceKHR GetHandle() const { return m_Surface; }

private:

	void Create(const uncanny::IWindow* pWindow, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);
	void Destroy(VkInstance vkInstance);

	[[nodiscard]] b8 IsPresentationSupported(FQueueFamilyIndex queueFamilyIndex) const;

private:

	VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
	VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_WINDOWSURFACE_H
