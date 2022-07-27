
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include "VulkanUtilities.h"


namespace uncanny
{


template<typename TProperties>
static void fillInRequiredProperties(const std::vector<const char*>& requiredProperties,
                                      std::vector<const char*>* pReturnProperties);


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);


b32 FRenderContextVulkan::createInstance() {
  UTRACE("Creating Vulkan Instance...");

  VK_CHECK( volkInitialize() );

  const u32 vulkanVersion{ retrieveVulkanApiVersion() };

  // Instance Layers
  std::vector<const char*> enabledLayers{};
  std::vector<const char*> requiredLayers{};
  if constexpr (U_VK_DEBUG) {
    UTRACE("Adding debug validation layers to VkInstance...");
    requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
  }
  fillInRequiredProperties<VkLayerProperties>(requiredLayers, &enabledLayers);

  // Instance Extensions
  std::vector<const char*> enabledExtensions{};
  std::vector<const char*> requiredExtensions{};
  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    UTRACE("Adding surface khr win32 extensions to VkInstance...");
    requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  }
  if constexpr (U_VK_DEBUG) {
    UTRACE("Adding debug utils extension to VkInstance...");
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
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

  if constexpr (U_VK_DEBUG) {
    UTRACE("Adding debug report callback to Vulkan...");

    auto vkCreateDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstanceVk,
                                                                  "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallbackFunc;
    VK_CHECK( vkCreateDebugUtilsMessengerEXT(mInstanceVk, &debugInfo, nullptr, &mDebugUtilsMsg) );
  }

  UDEBUG("Created Vulkan Instance, version {}.{}!", retrieveVulkanApiMajorVersion(vulkanVersion),
         retrieveVulkanApiMinorVersion(vulkanVersion));
  return UTRUE;
}


b32 FRenderContextVulkan::closeInstance() {
  UTRACE("Closing Vulkan Instance...");

  if (mDebugUtilsMsg != VK_NULL_HANDLE) {
    auto vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstanceVk,
                                                                   "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(mInstanceVk, mDebugUtilsMsg, nullptr);
  }

  vkDestroyInstance(mInstanceVk, nullptr);

  UDEBUG("Closed Vulkan Instance!");
  return UTRUE;
}


template<typename TProperties>
static void iterateOverAndLog(const std::vector<TProperties>& properties,
                              const char* (*pRetrieveFunc)(const TProperties&),
                              const char* areMandatory);


template<typename TProperties>
static const char* retrievePropertyName(const TProperties&);

template<typename TProperties>
static b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                        const std::vector<TProperties>& availableProperties);


template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };

template<typename T> struct is_vk_layer_properties : std::false_type { };
template<> struct is_vk_layer_properties<VkLayerProperties> : std::true_type { };


template<typename TProperties>
void fillInRequiredProperties(const std::vector<const char*>& requiredProperties,
                              std::vector<const char*>* pReturnProperties) {
  uint32_t count{ 0 };
  std::vector<TProperties> availableProperties{};

  if constexpr (is_vk_extension_properties<TProperties>::value) {
    VK_CHECK( vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr) );
    availableProperties.resize(count);
    VK_CHECK( vkEnumerateInstanceExtensionProperties(nullptr, &count, availableProperties.data()) );
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    VK_CHECK( vkEnumerateInstanceLayerProperties(&count, nullptr) );
    availableProperties.resize(count);
    VK_CHECK( vkEnumerateInstanceLayerProperties(&count, availableProperties.data()) );
  }
  else {
    UERROR("Given wrong TProperties type, cannot enable VkInstance layers and extensions!");
    return;
  }

  iterateOverAndLog(requiredProperties, retrievePropertyName, "REQUIRED");
  if constexpr (U_VK_DEBUG) {
    iterateOverAndLog(availableProperties, retrievePropertyName, "AVAILABLE");
  }

  for (const char* required : requiredProperties) {
    if (isRequiredPropertyAvailable(required, availableProperties)) {
      pReturnProperties->push_back(required);
    }
  }
  iterateOverAndLog(*pReturnProperties, retrievePropertyName, "RETURN");
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
b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                const std::vector<TProperties>& availableProperties) {
  auto isStringInPropertiesVector = [requiredProperty](const TProperties& property){
    return std::strcmp(retrievePropertyName(property), requiredProperty);
  };
  auto it{ std::find_if(availableProperties.begin(), availableProperties.end(),
                        isStringInPropertiesVector) };
  if (it != availableProperties.end()) {
    return UTRUE;
  }

  return UFALSE;
}


VkBool32 debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                           void* pUserData) {
  const auto error = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  const auto warning = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  const auto info = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  const auto verbose = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

  if (error) {
    printf("%s : %s\n", "ERROR", pCallbackData->pMessage);
  }
  else if (warning) {
    printf("%s : %s\n", "WARNING", pCallbackData->pMessage);
  }
  else if (info) {
    printf("%s : %s\n", "INFO", pCallbackData->pMessage);
  }
  else if (verbose) {
    printf("%s : %s\n", "VERBOSE", pCallbackData->pMessage);
  }

  return VK_FALSE;
}


}
