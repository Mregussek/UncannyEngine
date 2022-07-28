
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanDebugUtils.h"
#include "VulkanWindowSurface.h"
#include <utilities/Logger.h>


namespace uncanny
{


template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };

template<typename T> struct is_vk_layer_properties : std::false_type { };
template<> struct is_vk_layer_properties<VkLayerProperties> : std::true_type { };


template<typename TProperties>
static void ensureAllRequiredPropertiesAreAvailable(
    const std::vector<const char*>& requiredProperties);


template<typename TProperties>
static void iterateOverAndLog(const std::vector<TProperties>& properties,
                              const char* (*pRetrieveFunc)(const TProperties&),
                              const char* areMandatory);


template<typename TProperties>
static const char* retrievePropertyName(const TProperties&);

template<typename TProperties>
static b32 isRequiredPropertyInVector(const char* requiredProperty,
                                      const std::vector<TProperties>& vec);


b32 FRenderContextVulkan::createInstance() {
  UTRACE("Creating Vulkan Instance...");

  // Initializing volk -> loading all vulkan functions
  VkResult initializedVolk{ volkInitialize() };
  if (initializedVolk != VK_SUCCESS) {
    UFATAL("Could not initialize volk library along with Vulkan SDK, check you GPU drivers and "
           "downloaded Vulkan SDK");
    return UFALSE;
  }

  // Checking window system support for vulkan renderer
  b32 windowSystemSupportsVulkan{ windowSurfaceSupportVulkanAPI(mSpecs.pWindow) };
  if (not windowSystemSupportsVulkan) {
    UFATAL("Window system does not support vulkan renderer, so surface cannot be created!");
    return UFALSE;
  }

  // Instance Layers
  std::vector<const char*> requiredLayers{};
  if constexpr (U_VK_DEBUG) {
    getRequiredDebugInstanceLayers(&requiredLayers);
  }

  // Instance Extensions
  std::vector<const char*> requiredExtensions{};
  if constexpr (U_VK_DEBUG) {
    getRequiredDebugInstanceExtensions(&requiredExtensions);
  }
  getRequiredWindowSurfaceInstanceExtensions(mSpecs.pWindow, &requiredExtensions);

  // Log instance layers and extensions
  ensureAllRequiredPropertiesAreAvailable<VkLayerProperties>(requiredLayers);
  ensureAllRequiredPropertiesAreAvailable<VkExtensionProperties>(requiredExtensions);
  iterateOverAndLog(requiredLayers, retrievePropertyName, "Enable Instance Layers");
  iterateOverAndLog(requiredExtensions, retrievePropertyName, "Enable Instance Extensions");

  // Vulkan API version is needed
  const u32 vulkanVersion{ retrieveVulkanApiVersion() };

  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.apiVersion = vulkanVersion;
  appInfo.pApplicationName = mSpecs.pAppName;
  appInfo.applicationVersion = mSpecs.appVersion;
  appInfo.pEngineName = "Uncanny Engine";
  appInfo.engineVersion = mSpecs.engineVersion;

  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.pApplicationInfo = &appInfo;
  createInfo.ppEnabledLayerNames = requiredLayers.data();
  createInfo.enabledLayerCount = requiredLayers.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  createInfo.enabledExtensionCount = requiredExtensions.size();

  VkResult instanceCreated{ vkCreateInstance(&createInfo, nullptr, &mInstanceVk) };
  if (instanceCreated != VK_SUCCESS) {
    UFATAL("Could not create VkInstance, check your vulkan SDK and GPU drivers");
    return UFALSE;
  }

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


template<typename TProperties>
void ensureAllRequiredPropertiesAreAvailable(const std::vector<const char*>& requiredProperties) {
  u32 count{ 0 };
  std::vector<TProperties> availableProperties{};

  if constexpr (is_vk_extension_properties<TProperties>::value) {
    U_VK_ASSERT( vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr) );
    availableProperties.resize(count);
    U_VK_ASSERT( vkEnumerateInstanceExtensionProperties(nullptr, &count,
                                                        availableProperties.data()) );
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    U_VK_ASSERT( vkEnumerateInstanceLayerProperties(&count, nullptr) );
    availableProperties.resize(count);
    U_VK_ASSERT( vkEnumerateInstanceLayerProperties(&count, availableProperties.data()) );
  }
  else {
    UERROR("Given wrong TProperties type, cannot validate VkInstance layers and extensions!");
    return;
  }

  if constexpr (U_VK_DEBUG) {
    iterateOverAndLog(availableProperties, retrievePropertyName, "AVAILABLE");
  }

  for (const char* required : requiredProperties) {
    if (not isRequiredPropertyInVector(required, availableProperties)) {
      UTRACE("Property {} is not available!", required);
    }
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


template<typename TProperties> const char* retrievePropertyName(const TProperties& p) {
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


template<typename TProperties>
b32 isRequiredPropertyInVector(const char* requiredProperty,
                               const std::vector<TProperties>& vec) {
  auto it{ std::find_if(vec.begin(), vec.end(), [requiredProperty](TProperties property){
    return std::strcmp(retrievePropertyName(property), requiredProperty);
  })};
  if (it != vec.end()) {
    return UTRUE;
  }

  return UFALSE;
}


}
