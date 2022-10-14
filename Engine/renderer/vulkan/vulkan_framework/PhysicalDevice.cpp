
#include "PhysicalDevice.h"
#include "Utilities.h"
#include <utilities/Logger.h>
#if WIN32
#include <vulkan/vulkan_win32.h>
#endif


namespace uncanny::vkf {


template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };


static b32 physicalDeviceDoesNotHaveMandatoryDependencies(VkPhysicalDevice physicalDevice);


static b32 granularityHasProperValuesForGraphics(VkExtent3D granularity);


template<typename TProperties>
static b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                       const std::vector<TProperties>& availablePropertiesVector);


template<typename TProperties>
static const char* retrievePropertyName(const TProperties& properties);


static u32 findBestSuitableQueueFamilyForGraphics(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
                                                  VkInstance instance,
                                                  VkPhysicalDevice physicalDevice);


static b32 queueFamilySupportsPlatformPresentation(u32 queueFamilyIndex,
                                                   VkInstance instance,
                                                   VkPhysicalDevice physicalDevice);


static u32 findBestSuitableQueueFamilyForTransfer(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
                                                  u32 queueFamilyIgnoredIfPossible);


b32 FPhysicalDeviceVulkan::init(const FPhysicalDeviceInitDependenciesVulkan& deps) {
  UTRACE("Initializing physical device vulkan...");

  u32 devicesCount{ 0 };
  AssertResultVulkan( vkEnumeratePhysicalDevices(deps.instance, &devicesCount, nullptr) );
  std::vector<VkPhysicalDevice> physicalDevicesVector(devicesCount);
  AssertResultVulkan( vkEnumeratePhysicalDevices(deps.instance, &devicesCount, physicalDevicesVector.data()) );

  auto endIterator = std::remove_if(physicalDevicesVector.begin(), physicalDevicesVector.end(),
                                    physicalDeviceDoesNotHaveMandatoryDependencies);

  // Erase physical devices from vector that doesn't have mandatory deps...
  physicalDevicesVector.erase(endIterator, physicalDevicesVector.end());

  if (physicalDevicesVector.empty()) {
    UERROR("After erasing all physical devices that does not have mandatory dependencies, no devices left!");
    return UFALSE;
  }

  // Assigning first physical device with believe it will be best
  m_VkPhysicalDevice = physicalDevicesVector[0];

  // Retrieve queue families info...
  u32 queueFamilyCount{ 0 };
  vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesVector(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, queueFamilyPropertiesVector.data());

  m_QueueFamilyGraphics = findBestSuitableQueueFamilyForGraphics(queueFamilyPropertiesVector,
                                                                 deps.instance,
                                                                 m_VkPhysicalDevice);
  if (m_QueueFamilyGraphics == UUNUSED) {
    UERROR("Wrong graphics queue family returned!");
    return UFALSE;
  }
  m_VkQueueFamilyPropertiesGraphics = queueFamilyPropertiesVector[m_QueueFamilyGraphics];

  m_QueueFamilyTransfer = findBestSuitableQueueFamilyForTransfer(queueFamilyPropertiesVector,
                                                                 m_QueueFamilyGraphics);
  if (m_QueueFamilyTransfer == UUNUSED) {
    UERROR("Wrong transfer queue family returned!");
    return UFALSE;
  }
  m_VkQueueFamilyPropertiesTransfer = queueFamilyPropertiesVector[m_QueueFamilyTransfer];

  UDEBUG("Initialized physical device vulkan...");
  return UTRUE;
}


void FPhysicalDeviceVulkan::terminate() {
  UTRACE("Terminating physical device vulkan...");

  m_VkPhysicalDevice = VK_NULL_HANDLE;
  m_QueueFamilyGraphics = UUNUSED;
  m_QueueFamilyTransfer = UUNUSED;

  UDEBUG("Terminated physical device vulkan...");
}


b32 physicalDeviceDoesNotHaveMandatoryDependencies(VkPhysicalDevice physicalDevice) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);

  u32 extensionCount{ 0 };
  AssertResultVulkan( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
  std::vector<VkExtensionProperties> extensions(extensionCount);
  AssertResultVulkan( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data()) );

  UTRACE("Make sure that physical device {} is correct type", properties.deviceName);
  const b32 isDiscrete{ properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU };
  const b32 isIntegrated{ properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU };
  if (not (isDiscrete or isIntegrated)) {
    UTRACE("Physical device {} isn't discrete nor integrated GPU!", properties.deviceName);
    return UTRUE;
  }

  UTRACE("Make sure that physical device {} has swapchain support", properties.deviceName);
  if (not isRequiredPropertyAvailable(VK_KHR_SWAPCHAIN_EXTENSION_NAME, extensions)) {
    UTRACE("Physical device {} doesn't have swapchain support!", properties.deviceName);
    return UTRUE;
  }

  u32 queueFamilyCount{ 0 };
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesVector(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyPropertiesVector.data());

  b32 hasGraphicsQueueFamily{ UFALSE };
  b32 hasTransferQueueFamily{ UFALSE };
  for (u32 i = 0; i < queueFamilyCount; i++) {
    if (queueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      if (granularityHasProperValuesForGraphics(queueFamilyPropertiesVector[i].minImageTransferGranularity)) {
        hasGraphicsQueueFamily = UTRUE;
      }
    }
    if (queueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      hasTransferQueueFamily = UTRUE;
    }
  }

  if (not (hasGraphicsQueueFamily and hasTransferQueueFamily)) {
    UTRACE("Physical device {} does not have graphics or transfer queue family! Graphics {} Transfer {}",
           properties.deviceName, hasGraphicsQueueFamily, hasTransferQueueFamily);
    return UTRUE;
  }

  UDEBUG("Found physical device {} having all mandatory dependencies!", properties.deviceName);
  return UFALSE;
}


b32 granularityHasProperValuesForGraphics(VkExtent3D granularity) {
  if (granularity.width == 1 and granularity.height == 1 and granularity.depth == 1) {
    UTRACE("Min Image Transfer Granularity has correct values ({}, {}, {})!", granularity.width, granularity.height,
           granularity.depth);
    return UTRUE;
  }
  UTRACE("Min Image Transfer Granularity has wrong values ({}, {}, {})!", granularity.width, granularity.height,
         granularity.depth);
  return UFALSE;
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


template<typename TProperties>
const char* retrievePropertyName(const TProperties& properties) {
  if constexpr (is_vk_extension_properties<TProperties>::value) {
    return properties.extensionName;
  }
  else if constexpr (std::is_same<TProperties, const char*>::value) {
    return properties;
  }
  else {
    UERROR("Wrong property type given, could not retrieve its name!");
    AssertResultVulkan(VK_ERROR_UNKNOWN);
  }
}


u32 findBestSuitableQueueFamilyForGraphics(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
                                           VkInstance instance,
                                           VkPhysicalDevice physicalDevice) {
  UTRACE("Looking for graphics queue family...");
  for (u32 i = 0; i < queueFamilyProperties.size(); i++) {
    if (not (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      UTRACE("Queue Family {} does not have graphics support!", i);
      continue;
    }
    if (not (granularityHasProperValuesForGraphics(queueFamilyProperties[i].minImageTransferGranularity))) {
      UTRACE("Queue Family {} properties does not have min image transfer granularity proper values!", i);
      continue;
    }
    if (not (queueFamilySupportsPlatformPresentation(i, instance, physicalDevice))) {
      UTRACE("Queue Family {} does not support platform presentation!", i);
      continue;
    }
    UTRACE("Returning queue family {} as graphics!", i);
    return i;
  }

  UERROR("No graphics queue family index found, returning unused!");
  return UUNUSED;
}


b32 queueFamilySupportsPlatformPresentation(u32 queueFamilyIndex,
                                            VkInstance instance,
                                            VkPhysicalDevice physicalDevice) {
  if constexpr (WIN32) {
    auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    VkBool32 result{ vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex) };
    if (result) {
      UTRACE("Presentation supported by Win32 Platform!");
      return UTRUE;
    }

    UERROR("Presentation is not supported by Win32 Platform!");
    return UFALSE;
  }
  else {
    UERROR("Not supported platform, cannot check platform presentation!");
    return VK_FALSE;
  }
}


u32 findBestSuitableQueueFamilyForTransfer(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
                                           u32 queueFamilyIgnoredIfPossible) {
  UTRACE("Looking for transfer queue family...");
  for (u32 i = 0; i < queueFamilyProperties.size(); i++) {
    if (not (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)) {
      UTRACE("Queue Family {} does not support transfer!", i);
      continue;
    }
    if (queueFamilyIgnoredIfPossible == i) {
      UTRACE("Queue family {} ignored!", i);
      continue;
    }

    UTRACE("Found transfer queue family index {}!", i);
    return i;
  }

  if (queueFamilyProperties.size() == 1 and queueFamilyIgnoredIfPossible == 0) {
    UTRACE("Found queue family index 0 ignored for transfer, but there is only one queue family available, returning");
    return 0;
  }

  UERROR("No transfer queue family index found, returning unused!");
  return UUNUSED;
}


}
