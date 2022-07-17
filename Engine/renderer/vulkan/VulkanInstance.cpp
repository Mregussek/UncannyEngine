
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include "VulkanUtilities.h"


namespace uncanny
{


static u32 retrieveVulkanVersion();

template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };

template<typename T> struct is_vk_layer_properties : std::false_type { };
template<> struct is_vk_layer_properties<VkLayerProperties> : std::true_type { };


template<typename TProperties>
static void fillInRequiredProperties(const std::vector<const char*>& requiredProperties,
                                      std::vector<const char*>* pReturnProperties);


b32 FRenderContextVulkan::createInstance() {
  UTRACE("Creating Vulkan Instance...");

  VK_CHECK( volkInitialize() );

  const u32 vulkanVersion{ retrieveVulkanVersion() };

  // Instance Layers
  std::vector<const char*> enabledLayers;
  std::vector<const char*> requiredLayers{};
  if constexpr (U_VK_DEBUG) {
    requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
  }
  fillInRequiredProperties<VkLayerProperties>(requiredLayers, &enabledLayers);

  // Instance Extensions
  std::vector<const char*> enabledExtensions;
  std::vector<const char*> requiredExtensions{ VK_KHR_SURFACE_EXTENSION_NAME };
#ifdef VK_KHR_WIN32_SURFACE_EXTENSION_NAME
  // there is needed if-macro, as when this ext is not defined there is no variable
  requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
  fillInRequiredProperties<VkExtensionProperties>(requiredExtensions, &enabledExtensions);

  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.apiVersion = vulkanVersion;
  appInfo.pApplicationName = mSpecs.pAppName;
  appInfo.applicationVersion = mSpecs.appVersion;
  appInfo.pEngineName = "Uncanny Engine";
  appInfo.engineVersion = mSpecs.engineVersion;

  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.pApplicationInfo = &appInfo;
  createInfo.ppEnabledLayerNames = enabledLayers.data();
  createInfo.enabledLayerCount = enabledLayers.size();
  createInfo.ppEnabledExtensionNames = enabledExtensions.data();
  createInfo.enabledExtensionCount = enabledExtensions.size();

  VK_CHECK( vkCreateInstance(&createInfo, nullptr, &mInstanceVk) );

  volkLoadInstance(mInstanceVk);

  UDEBUG("Created Vulkan Instance, version {}.{}!", VK_API_VERSION_MAJOR(vulkanVersion),
         VK_API_VERSION_MINOR(vulkanVersion));
  return UTRUE;
}


b32 FRenderContextVulkan::closeInstance() {
  UTRACE("Closing Vulkan Instance...");

  vkDestroyInstance(mInstanceVk, nullptr);

  UDEBUG("Closed Vulkan Instance!");
  return UTRUE;
}


u32 retrieveVulkanVersion() {
  const std::array<u32, 4> vulkanVersionsArray{
    VK_API_VERSION_1_3, VK_API_VERSION_1_2, VK_API_VERSION_1_1, VK_API_VERSION_1_0
  };

  for(u32 version : vulkanVersionsArray) {
    const u32 variant{ VK_API_VERSION_VARIANT(version) };
    const u32 major{ VK_API_VERSION_MAJOR(version) };
    const u32 minor{ VK_API_VERSION_MINOR(version) };
    const VkResult result{ vkEnumerateInstanceVersion(&version) };
    UTRACE("Logging VkResult for vkEnumerateInstanceVersion Variant {} Version {}.{} -> {}", variant,
           major, minor, result);
    if (result == VK_SUCCESS) {
      return version;
    }
  }

  UFATAL("Any Vulkan API is not supported on this device, check your drivers!");
  return 0;
  // TODO: Handle no support for Vulkan API, remember that vkEnumerateInstanceVersion always
  //  return VK_SUCCESS, even with 0.0.0 version
  //  Variant.Major.Minor.Patch -> currently patch does not mean nothing!
}


template<typename TProperties>
static void iterateOverAndLog(const std::vector<TProperties>& properties,
                              const char* (*pRetrieveFunc)(const TProperties&),
                              const char* areMandatory);


template<typename TProperties>
static const char* retrievePropertyName(const TProperties&);


template<typename TProperties>
void fillInRequiredProperties(const std::vector<const char*>& requiredProperties,
                              std::vector<const char*>* pReturnProperties) {
  uint32_t count{ 0 };
  std::vector<TProperties> availableProperties{};
  iterateOverAndLog(requiredProperties, retrievePropertyName, "REQUIRED");

  if constexpr (is_vk_extension_properties<TProperties>::value) {
    VK_CHECK( vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr) );
    availableProperties.resize(count);
    VK_CHECK( vkEnumerateInstanceExtensionProperties(nullptr, &count, availableProperties.data()) );
    iterateOverAndLog(availableProperties, retrievePropertyName, "AVAILABLE");
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    VK_CHECK( vkEnumerateInstanceLayerProperties(&count, nullptr) );
    availableProperties.resize(count);
    VK_CHECK( vkEnumerateInstanceLayerProperties(&count, availableProperties.data()) );
    iterateOverAndLog(availableProperties, retrievePropertyName, "AVAILABLE");
  }
  else {
    UERROR("Given wrong TProperties type, cannot enable VkInstance layers and extensions!");
    return;
  }
}


template<typename TProperties>
void iterateOverAndLog(const std::vector<TProperties>& properties,
                       const char* (*pRetrieveFunc)(const TProperties&),
                       const char* areMandatory) {
  UTRACE("{} Instance {} properties:", areMandatory, typeid(TProperties).name());
  for (const TProperties& property : properties) {
    std::cout << pRetrieveFunc(property) << ", ";
  }
  std::cout << '\n';
}


template<typename TProperties>
const char* retrievePropertyName(const TProperties& p) {
  if constexpr (is_vk_extension_properties<TProperties>::value) {
    return p.extensionName;
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    return p.layerName;
  }
  else if constexpr (std::is_same<TProperties, const char*>::value) {
    return p;
  }
  else {
    return "Unknown";
  }
}


}
