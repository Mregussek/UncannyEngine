
#include "ContextVulkan.h"
#include "VulkanDebugUtils.h"
#include "VulkanWindowSurface.h"
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


template<typename TProperties> static b32 areRequiredPropertiesAvailable(
    const std::vector<const char*>& requiredProperties);


template<typename TProperties> static void iterateOverAndLog(
    const std::vector<TProperties>& properties, const char* (*pRetrieveFunc)(const TProperties&),
    const char* areMandatory);


template<typename TProperties> static const char* retrievePropertyName(const TProperties&);


template<typename TProperties> static b32 isRequiredPropertyInVector(
    const char* requiredProperty, const std::vector<TProperties>& vec);


b32 FRenderContextVulkan::createInstance() {
  UTRACE("Creating Vulkan Instance...");

  // Checking window system support for vulkan renderer before any initialization
  b32 windowSystemSupportsVulkan{ windowSurfaceSupportVulkanAPI(mSpecs.pWindow) };
  if (not windowSystemSupportsVulkan) {
    UFATAL("Window system does not support vulkan renderer, so surface cannot be created!");
    return UFALSE;
  }

  // Initializing volk -> loading all vulkan functions
  VkResult initializedVolk{ volkInitialize() };
  if (initializedVolk != VK_SUCCESS) {
    UFATAL("Could not initialize volk library along with Vulkan SDK, check you GPU drivers and "
           "downloaded Vulkan SDK");
    return UFALSE;
  }

  // Is Vulkan API version available
  u32 apiVersion{ UVERSION_UNDEFINED };
  VkResult isVersionAvailable{ vkEnumerateInstanceVersion(&apiVersion) };
  if (isVersionAvailable != VK_SUCCESS or mInstanceDependencies.vulkanApiVersion >= apiVersion) {
    UFATAL("Vulkan version {}.{} is not available, cannot start vulkan renderer!",
           VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion));
    return UFALSE;
  }

  // Instance Layers
  std::vector<const char*> requiredLayers{};
  if constexpr (U_VK_DEBUG) {
    getRequiredDebugInstanceLayers(&requiredLayers);
  }

  // TODO: layers existence is validated during vkCreateInstance and if is no available
  //  return code is VK_ERROR_LAYER_NOT_PRESENT, make sure if checking it here is proper
  // Validate all required layers
  if (not areRequiredPropertiesAvailable<VkLayerProperties>(requiredLayers)) {
    UERROR("Some required instance layers are not available, cannot start vulkan renderer!");
    return UFALSE;
  }
  iterateOverAndLog(requiredLayers, retrievePropertyName, "Enable Instance Layers");

  // Instance Extensions
  std::vector<const char*> requiredExtensions{};
  if constexpr (U_VK_DEBUG) {
    getRequiredDebugInstanceExtensions(&requiredExtensions);
  }
  getRequiredWindowSurfaceInstanceExtensions(mSpecs.pWindow, &requiredExtensions);

  // TODO: extensions support is validated during vkCreateInstance and if is no available
  //  return code is VK_ERROR_EXTENSION_NOT_PRESENT, make sure if checking it here is proper
  // Validate all required extensions
  if (not areRequiredPropertiesAvailable<VkExtensionProperties>(requiredExtensions)) {
    UERROR("Some required instance extensions are not available, cannot start vulkan renderer!");
    return UFALSE;
  }
  iterateOverAndLog(requiredExtensions, retrievePropertyName, "Enable Instance Extensions");

  // Create infos...
  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.pNext = nullptr;
  appInfo.pApplicationName = mSpecs.pAppName;
  appInfo.applicationVersion = mSpecs.appVersion;
  appInfo.pEngineName = "Uncanny Engine";
  appInfo.engineVersion = mSpecs.engineVersion;
  appInfo.apiVersion = apiVersion;

  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = requiredLayers.size();
  createInfo.ppEnabledLayerNames = requiredLayers.data();
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkResult instanceCreated{ vkCreateInstance(&createInfo, nullptr, &mVkInstance) };
  if (instanceCreated != VK_SUCCESS) {
    UFATAL("Could not create VkInstance, check your vulkan SDK and GPU drivers");
    return UFALSE;
  }

  volkLoadInstance(mVkInstance);

  UDEBUG("Created Vulkan Instance, version {}.{}!", VK_API_VERSION_MAJOR(apiVersion),
         VK_API_VERSION_MINOR(apiVersion));
  return UTRUE;
}


b32 FRenderContextVulkan::closeInstance() {
  UTRACE("Closing Vulkan Instance...");

  if (mVkInstance != VK_NULL_HANDLE) {
    UTRACE("Destroying vulkan instance...");
    vkDestroyInstance(mVkInstance, nullptr);
  }
  else {
    UWARN("Instance is not closed, as it wasn't created!");
  }

  UDEBUG("Closed Vulkan Instance!");
  return UTRUE;
}


template<typename TProperties>
b32 areRequiredPropertiesAvailable(const std::vector<const char*>& requiredProperties) {
  UTRACE("Ensuring all required properties are available for VkInstance...");

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
    return UFALSE;
  }

  if constexpr (U_VK_DEBUG) {
    iterateOverAndLog(availableProperties, retrievePropertyName, "AVAILABLE");
  }

  for (const char* required : requiredProperties) {
    if (not isRequiredPropertyInVector(required, availableProperties)) {
      UTRACE("Property {} is not available!", required);
      return UFALSE;
    }
  }

  return UTRUE;
}


template<typename TProperties>
void iterateOverAndLog(const std::vector<TProperties>& properties,
                       const char* (*pRetrieveFunc)(const TProperties&),
                       const char* areMandatory) {
  UTRACE("{} Instance {} properties:", areMandatory, typeid(TProperties).name());
  for (TProperties property : properties) {
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
    return std::strcmp(retrievePropertyName(property), requiredProperty) == 0;
  })};
  if (it != vec.end()) {
    UTRACE("Required property {} is in properties vector!", requiredProperty);
    return UTRUE;
  }

  UTRACE("Required property {} does not exist in properties vector!", requiredProperty);
  return UFALSE;
}


}
