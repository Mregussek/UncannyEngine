
#ifndef UNCANNYENGINE_VULKANWINDOWSURFACE_H
#define UNCANNYENGINE_VULKANWINDOWSURFACE_H


namespace uncanny
{


b32 windowSurfaceSupportVulkanAPI(FWindow* pWindow);


void getRequiredWindowSurfaceInstanceExtensions(
    FWindow* pWindow, std::vector<const char*>* pRequiredExtensions);


b32 windowSurfaceSupportPresentationOnPhysicalDevice(
    FWindow* pWindow, VkInstance instance, VkPhysicalDevice physicalDevice, u32 queueFamilyIndex);


}


#endif //UNCANNYENGINE_VULKANWINDOWSURFACE_H
