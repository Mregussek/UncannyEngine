
#ifndef UNCANNYENGINE_VULKANWINDOWSURFACE_H
#define UNCANNYENGINE_VULKANWINDOWSURFACE_H


#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


class FWindow;



b32 windowSurfaceSupportVulkanAPI(const FWindow* pWindow);


void getRequiredWindowSurfaceInstanceExtensions(
    const FWindow* pWindow, std::vector<const char*>* pRequiredExtensions);


b32 windowSurfaceSupportPresentationOnPhysicalDevice(
    const FWindow* pWindow, VkInstance instance, VkPhysicalDevice physicalDevice,
    u32 queueFamilyIndex);


}


#endif //UNCANNYENGINE_VULKANWINDOWSURFACE_H
