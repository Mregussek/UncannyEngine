
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>


#define VK_CHECK(call) \
    do { \
        const VkResult result = call; \
    } while(0)


namespace uncanny
{


template<u32 TRequiredVersion>
static u32 retrieveVulkanVersion();


b32 FRenderContextVulkan::createInstance() {
  UTRACE("Creating Vulkan Instance...");

  VK_CHECK( volkInitialize() );

  displayInfoAboutVersion(mSpecs.engineVersion, "EngineVersion");
  u32 vulkanVersion{ retrieveVulkanVersion<VK_API_VERSION_1_3>() };

  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.apiVersion = vulkanVersion;
  appInfo.pApplicationName = mSpecs.pAppName;
  appInfo.applicationVersion = mSpecs.appVersion;
  appInfo.pEngineName = "Uncanny Engine";
  appInfo.engineVersion = mSpecs.engineVersion;

  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.pApplicationInfo = &appInfo;
  createInfo.ppEnabledLayerNames = nullptr;
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledExtensionNames = nullptr;
  createInfo.enabledExtensionCount = 0;

  VK_CHECK( vkCreateInstance(&createInfo, nullptr, &mInstanceVk) );

  volkLoadInstance(mInstanceVk);

  UDEBUG("Created Vulkan Instance!");
  return UTRUE;
}


b32 FRenderContextVulkan::closeInstance() {
  UTRACE("Closing Vulkan Instance...");

  vkDestroyInstance(mInstanceVk, nullptr);

  UDEBUG("Closed Vulkan Instance!");
  return UTRUE;
}


template<u32 TRequiredVersion>
u32 retrieveVulkanVersion() {
  const u32 variant{ VK_API_VERSION_VARIANT(TRequiredVersion) };
  const u32 major{ VK_API_VERSION_MAJOR(TRequiredVersion) };
  const u32 minor{ VK_API_VERSION_MINOR(TRequiredVersion) };
  const u32 patch{ VK_API_VERSION_PATCH(TRequiredVersion) };
  UINFO("Chosen Vulkan API version: Variant {} Major {} Minor {} Patch {}", variant,  major, minor,
        patch);
  return VK_MAKE_API_VERSION(variant, major, minor, patch);
}


}
