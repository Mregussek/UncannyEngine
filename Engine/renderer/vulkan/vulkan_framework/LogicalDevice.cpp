
#include "LogicalDevice.h"
#include "Utilities.h"
#include <renderer/vulkan/vulkan_renderer/RendererVulkan.h>
#include <utilities/Logger.h>


namespace uncanny::vkf
{


template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };


static void ensureRequiredPropertiesAvailable(VkPhysicalDevice physicalDevice,
                                              const std::vector<const char*>& requiredProperties);


static void composeStringFromPropertiesVector(const std::vector<VkExtensionProperties>& properties,
                                              std::string* pOut);


static b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                       const std::vector<VkExtensionProperties>& availablePropertiesVector);


b32 FLogicalDeviceVulkan::init(const FLogicalDeviceInitDependenciesVulkan& deps) {
  UTRACE("Initializing logical device vulkan...");

  // Device Extensions...
  std::vector<const char*> requiredExtensions{};
  FRendererVulkan::getRequiredExtensions(&requiredExtensions);

  // TODO: extensions support is validated during vkCreateDevice and if is no available
  //  return code is VK_ERROR_EXTENSION_NOT_PRESENT, make sure if checking it here is proper
  ensureRequiredPropertiesAvailable(deps.physicalDevice, requiredExtensions);

  // TODO: make sure if all physical device features are required
  // Querying physical device features and passing it to create info
  VkPhysicalDeviceFeatures physicalDeviceFeatures{};
  vkGetPhysicalDeviceFeatures(deps.physicalDevice, &physicalDeviceFeatures);

  if constexpr (not ENABLE_DEBUGGING_RENDERER) {
    UTRACE("As it is release build, disabling robustBufferAccess!");
    physicalDeviceFeatures.robustBufferAccess = UFALSE;
  }

  // Check proper value for length of device queue create infos...
  u32 deviceQueuesLength{ UUNUSED };
  if (deps.queueFamilyIndexGraphics == deps.queueFamilyIndexTransfer) {
    deviceQueuesLength = 1;
  }
  else {
    deviceQueuesLength = 2;
  }

  f32 priorities[3]{ 1.f };
  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoVector(deviceQueuesLength);

  // Create Queue Family for graphics...
  if (deviceQueueCreateInfoVector.size() >= 1) {
    deviceQueueCreateInfoVector[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfoVector[0].pNext = nullptr;
    deviceQueueCreateInfoVector[0].flags = 0;
    deviceQueueCreateInfoVector[0].queueFamilyIndex = deps.queueFamilyIndexGraphics;
    deviceQueueCreateInfoVector[0].queueCount = deps.queueFamilyPropertiesGraphics.queueCount;
    deviceQueueCreateInfoVector[0].pQueuePriorities = priorities;
  }

  // Create Queue Family for transfer if possible...
  if (deviceQueueCreateInfoVector.size() >= 2) {
    deviceQueueCreateInfoVector[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfoVector[1].pNext = nullptr;
    deviceQueueCreateInfoVector[1].flags = 0;
    deviceQueueCreateInfoVector[1].queueFamilyIndex = deps.queueFamilyIndexTransfer;
    deviceQueueCreateInfoVector[1].queueCount = deps.queueFamilyPropertiesTransfer.queueCount;
    deviceQueueCreateInfoVector[1].pQueuePriorities = priorities;
  }

  VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.queueCreateInfoCount = (u32)deviceQueueCreateInfoVector.size();
  createInfo.pQueueCreateInfos = deviceQueueCreateInfoVector.data();
  createInfo.enabledLayerCount = 0;             // deprecated!
  createInfo.ppEnabledLayerNames = nullptr;    // deprecated!
  createInfo.enabledExtensionCount = requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();
  createInfo.pEnabledFeatures = &physicalDeviceFeatures;

  VkResult result{ vkCreateDevice(deps.physicalDevice, &createInfo, nullptr, &m_VkDevice) };
  if (result != VK_SUCCESS) {
    UERROR("Cannot create logical device vulkan!");
    return UFALSE;
  }

  UDEBUG("Initialized logical device vulkan!");
  return UTRUE;
}


void FLogicalDeviceVulkan::terminate() {
  UTRACE("Terminating logical device vulkan...");

  if (m_VkDevice != VK_NULL_HANDLE) {
    UTRACE("Destroying vulkan logical device...");
    vkDeviceWaitIdle(m_VkDevice);
    vkDestroyDevice(m_VkDevice, nullptr);
  }
  else {
    UWARN("Logical device is not created, so it won't be closed!");
  }

  UDEBUG("Terminated logical device vulkan!");
}


void FLogicalDeviceVulkan::waitIdle() const {
  UTRACE("Logical device is waiting idle till all queues will be done with its commands...");
  vkDeviceWaitIdle(m_VkDevice);
}


void ensureRequiredPropertiesAvailable(VkPhysicalDevice physicalDevice,
                                       const std::vector<const char*>& requiredProperties) {
  UTRACE("Ensuring all required {} are available for vulkan instance...", typeid(VkExtensionProperties).name());
  u32 count{ 0 };
  AssertResultVulkan( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr) );
  std::vector<VkExtensionProperties> availableProperties(count);
  AssertResultVulkan( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, availableProperties.data()) );

  if constexpr (ENABLE_DEBUGGING_RENDERER) {
    std::string propertiesString{};
    composeStringFromPropertiesVector(availableProperties, &propertiesString);
    UTRACE("Available {}:\n{}", typeid(VkExtensionProperties).name(), propertiesString);
  }

  for (const char* requiredProperty : requiredProperties) {
    if (not isRequiredPropertyAvailable(requiredProperty, availableProperties)) {
      UERROR("{} {} is not available!", typeid(VkExtensionProperties).name(), requiredProperty);
      AssertResultVulkan(VK_ERROR_UNKNOWN);
    }
  }
}


void composeStringFromPropertiesVector(const std::vector<VkExtensionProperties>& properties,
                                       std::string* pOut) {
  *pOut = {};
  u32 i = 0;
  for (VkExtensionProperties property : properties) {
    *pOut += std::string(property.extensionName) + ", ";
    i++;
    if (i % 3 == 0) {
      i = 0;
      *pOut += "\n";
    }
  }
}


b32 isRequiredPropertyAvailable(const char* requiredProperty,
                                const std::vector<VkExtensionProperties>& availablePropertiesVector) {
  auto it{ std::find_if(availablePropertiesVector.begin(), availablePropertiesVector.end(),
                        [requiredProperty](VkExtensionProperties property) {
    return std::strcmp(property.extensionName, requiredProperty) == 0;
  })};
  if (it != availablePropertiesVector.end()) {
    UTRACE("Required {} {} is in properties vector!", typeid(VkExtensionProperties).name(), requiredProperty);
    return UTRUE;
  }

  UTRACE("Required {} {} does not exist in properties vector!", typeid(VkExtensionProperties).name(), requiredProperty);
  return UFALSE;
}


}
