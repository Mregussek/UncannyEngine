
#include "Instance.h"
#include "DebugUtils.h"
#include "Utilities.h"
#include <utilities/Logger.h>
#include <window/Window.h>
#include <window/glfw/WindowGLFW.h>


namespace uncanny::vkf
{


template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };

template<typename T> struct is_vk_layer_properties : std::false_type { };
template<> struct is_vk_layer_properties<VkLayerProperties> : std::true_type { };


template<typename TProperties>
static void ensureRequiredPropertiesAvailable(const std::vector<const char*>& requiredProperties);


template<typename TProperties>
static void composeStringFromPropertiesVector(const std::vector<TProperties>& properties, std::string* pOut);


template<typename TProperties>
static const char* retrievePropertyName(const TProperties& properties);


template<typename TProperties>
static b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                       const std::vector<TProperties>& availablePropertiesVector);


b32 FInstanceVulkan::init(const FInstanceInitDependenciesVulkan& deps) {
  UTRACE("Initializing instance vulkan...");

  // Initializing volk -> loading all vulkan functions
  VkResult initializedVolk{ volkInitialize() };
  if (initializedVolk != VK_SUCCESS) {
    UFATAL("Could not initialize volk library along with Vulkan SDK, check you GPU drivers and "
           "downloaded Vulkan SDK");
    return UFALSE;
  }

  // checking available vulkan version
  u32 apiVersion{ UVERSION_UNDEFINED };
  VkResult isVersionAvailable{ vkEnumerateInstanceVersion(&apiVersion) };
  if (isVersionAvailable != VK_SUCCESS or deps.expectedVulkanApiVersion >= apiVersion) {
    UFATAL("Vulkan version {}.{} is available, expected {}.{}, cannot start vulkan renderer!",
           VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion),
           VK_API_VERSION_MAJOR(deps.expectedVulkanApiVersion), VK_API_VERSION_MINOR(deps.expectedVulkanApiVersion));
    return UFALSE;
  }

  // Instance Layers
  std::vector<const char*> requiredLayers{};
  if constexpr (ENABLE_DEBUGGING_RENDERER) {
    emplaceRequiredDebugInstanceLayers(&requiredLayers);
  }
  // TODO: layers existence is validated during vkCreateInstance and if is no available
  //  return code is VK_ERROR_LAYER_NOT_PRESENT, make sure if checking it here is proper
  ensureRequiredPropertiesAvailable<VkLayerProperties>(requiredLayers);

  // Instance Extensions
  std::vector<const char*> requiredExtensions{};
  if constexpr (ENABLE_DEBUGGING_RENDERER) {
    emplaceRequiredDebugInstanceExtensions(&requiredExtensions);
  }
  if (deps.pWindow->getLibrary() == EWindowLibrary::GLFW) {
    emplaceRequiredGLFWWindowSurfaceInstanceExtensions(deps.pWindow, &requiredExtensions);
  }
  // TODO: extensions support is validated during vkCreateInstance and if is no available
  //  return code is VK_ERROR_EXTENSION_NOT_PRESENT, make sure if checking it here is proper
  ensureRequiredPropertiesAvailable<VkExtensionProperties>(requiredExtensions);

  // Actual create infos...
  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.pNext = nullptr;
  appInfo.pApplicationName = deps.appName;
  appInfo.applicationVersion = deps.appVersion;
  appInfo.pEngineName = "Uncanny Engine";
  appInfo.engineVersion = deps.engineVersion;
  appInfo.apiVersion = apiVersion;

  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = requiredLayers.size();
  createInfo.ppEnabledLayerNames = requiredLayers.data();
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkResult instanceCreated{ vkCreateInstance(&createInfo, nullptr, &m_VkInstance) };
  if (instanceCreated != VK_SUCCESS) {
    // We don't want to assert instance creation as it is important result and can be handled with changing Graphics API
    UFATAL("Could not create VkInstance, check your vulkan SDK and GPU drivers");
    return UFALSE;
  }

  // Loading volk...
  volkLoadInstance(m_VkInstance);

  UDEBUG("Initialized vulkan instance!");
  return UTRUE;
}


void FInstanceVulkan::terminate() {
  UTRACE("Terminating instance vulkan...");

  if (m_VkInstance != VK_NULL_HANDLE) {
    UTRACE("Destroying vulkan instance...");
    vkDestroyInstance(m_VkInstance, nullptr);
  }
  else {
    UWARN("vulkan instance is not closed, as it wasn't created!");
  }

  UDEBUG("Terminated vulkan instance!");
}


template<typename TProperties>
void ensureRequiredPropertiesAvailable(const std::vector<const char*>& requiredProperties) {
  UTRACE("Ensuring all required {} are available for vulkan instance...", typeid(TProperties).name());
  u32 count{ 0 };
  std::vector<TProperties> availableProperties{};

  if constexpr (is_vk_extension_properties<TProperties>::value) {
    AssertResultVulkan( vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr) );
    availableProperties.resize(count);
    AssertResultVulkan( vkEnumerateInstanceExtensionProperties(nullptr, &count, availableProperties.data()) );
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    AssertResultVulkan( vkEnumerateInstanceLayerProperties(&count, nullptr) );
    availableProperties.resize(count);
    AssertResultVulkan( vkEnumerateInstanceLayerProperties(&count, availableProperties.data()) );
  }
  else {
    UERROR("Given wrong TProperties type, cannot validate VkInstance layers and extensions!");
    AssertResultVulkan(VK_ERROR_UNKNOWN);
  }

  if constexpr (ENABLE_DEBUGGING_RENDERER) {
    std::string propertiesString{};
    composeStringFromPropertiesVector(availableProperties, &propertiesString);
    UTRACE("Available {}:\n{}", typeid(TProperties).name(), propertiesString);
  }

  for (const char* requiredProperty : requiredProperties) {
    if (not isRequiredPropertyAvailable(requiredProperty, availableProperties)) {
      UERROR("{} {} is not available!", typeid(TProperties).name(), requiredProperty);
      AssertResultVulkan(VK_ERROR_UNKNOWN);
    }
  }
}


template<typename TProperties>
void composeStringFromPropertiesVector(const std::vector<TProperties>& properties, std::string* pOut) {
  *pOut = {};
  u32 i = 0;

  for (TProperties property : properties) {
    *pOut += std::string(retrievePropertyName(property)) + ", ";
    i++;
    if (i % 3 == 0) {
      i = 0;
      *pOut += "\n";
    }
  }
}


template<typename TProperties>
const char* retrievePropertyName(const TProperties& properties) {
  if constexpr (is_vk_extension_properties<TProperties>::value) {
    return properties.extensionName;
  }
  else if constexpr (is_vk_layer_properties<TProperties>::value) {
    return properties.layerName;
  }
  else if constexpr (std::is_same<TProperties, const char*>::value) {
    return properties;
  }
  else {
    UERROR("Wrong property type given, could not retrieve its name!");
    AssertResultVulkan(VK_ERROR_UNKNOWN);
  }
}


template<typename TProperties>
b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                const std::vector<TProperties>& availablePropertiesVector) {
  auto it{ std::find_if(availablePropertiesVector.begin(), availablePropertiesVector.end(),
                        [requiredProperty](TProperties property) {
    return std::strcmp(retrievePropertyName(property), requiredProperty) == 0;
  })};
  if (it != availablePropertiesVector.end()) {
    UTRACE("Required {} {} is in properties vector!", typeid(TProperties).name(), requiredProperty);
    return UTRUE;
  }

  UTRACE("Required {} {} does not exist in properties vector!", typeid(TProperties).name(), requiredProperty);
  return UFALSE;
}


}
