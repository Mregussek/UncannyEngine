
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include "VulkanUtilities.h"


namespace uncanny
{


static b32 pickSuitableDevice(const std::vector<VkPhysicalDevice>& devicesVector,
                               VkPhysicalDevice* pRtnDevice, u32* pRtnQueueFamilyIndex,
                               VkQueueFamilyProperties* pRtnQueueFamilyProperties);


static void displayPhysicalDeviceData(const VkPhysicalDeviceProperties& deviceProperties,
                                      const VkPhysicalDeviceFeatures& deviceFeatures,
                                      const VkQueueFamilyProperties& queueFamilyProperties,
                                      u32 queueFamilyIndex=VK_QUEUE_FAMILY_IGNORED);


b32 FRenderContextVulkan::createPhysicalDevice() {
  UTRACE("Creating Physical Device...");

  u32 devicesCount{ 0 };
  VK_CHECK( vkEnumeratePhysicalDevices(mInstanceVk, &devicesCount, nullptr) );

  std::vector<VkPhysicalDevice> devicesVector(devicesCount);
  VK_CHECK( vkEnumeratePhysicalDevices(mInstanceVk, &devicesCount, devicesVector.data()) );

  b32 pickedDevice{ pickSuitableDevice(devicesVector, &mPhysicalDeviceVk, &mQueueFamilyIndex,
                                       &mQueueFamilyProperties) };
  if (not pickedDevice) {
    UFATAL("Could not pick suitable VkPhysicalDevice! There is no integrated GPU nor dedicated, "
           "there is no GPU supporting graphics nor platform presentation");
    return UFALSE;
  }

  vkGetPhysicalDeviceProperties(mPhysicalDeviceVk, &mPhysicalDeviceProperties);
  vkGetPhysicalDeviceFeatures(mPhysicalDeviceVk, &mPhysicalDeviceFeatures);

  displayPhysicalDeviceData(mPhysicalDeviceProperties, mPhysicalDeviceFeatures,
                            mQueueFamilyProperties, mQueueFamilyIndex);

  UDEBUG("Created Physical Device!");
  return UTRUE;
}


static std::pair<u32, VkQueueFamilyProperties> pickSuitableQueueFamilyIndex(
    u32 queuesCount, VkPhysicalDevice device,
    const std::vector<VkQueueFamilyProperties>& propertiesVector);


b32 pickSuitableDevice(const std::vector<VkPhysicalDevice>& devicesVector,
                       VkPhysicalDevice* pRtnDevice, u32* pRtnQueueFamilyIndex,
                       VkQueueFamilyProperties* pRtnQueueFamilyProperties) {
  UTRACE("Searching for suitable physical device...");
  for (VkPhysicalDevice device : devicesVector) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    displayPhysicalDeviceData(deviceProperties, deviceFeatures, { 0 });

    b32 isIntegratedGPU{ deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU };
    b32 isDiscreteGPU{ deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU };

    if (not (isIntegratedGPU or isDiscreteGPU)) {
      continue;
    }

    // TODO: add VkPhysicalDeviceProperties.limits check
    // properties.limits ---> VkPhysicalDeviceLimits (check this for some parameters)



    // TODO: add VkPhysicalDeviceFeatures check
    // deviceFeatures ---> (check this for some shaders support, draw support etc.)

    u32 queuesCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesCount, nullptr);

    std::vector<VkQueueFamilyProperties> queuesFamilyPropertiesVector(queuesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesCount, queuesFamilyPropertiesVector.data());

    auto [queueFamilyIndex, queueFamilyProperties] = pickSuitableQueueFamilyIndex(queuesCount,
                                                                                  device,
                                                                                  queuesFamilyPropertiesVector);
    if (queueFamilyIndex == VK_QUEUE_FAMILY_IGNORED) {
      continue;
    }

    *pRtnDevice = device;
    *pRtnQueueFamilyIndex = queueFamilyIndex;
    *pRtnQueueFamilyProperties = queueFamilyProperties;
    UDEBUG("Found suitable physical device!");
    return UTRUE;
  }

  return UFALSE;
}


static VkBool32 supportsPlatformPresentation(VkPhysicalDevice physicalDevice, u32 familyIndex);


enum class EQueueFamilyOperationsType {
  GRAPHICS, COMPUTE, TRANSFER, SPARSE_BINDING
};


template<EQueueFamilyOperationsType TOperationsType>
static b32 supportsOperations(VkQueueFlags queueFlags);


std::pair<u32, VkQueueFamilyProperties> pickSuitableQueueFamilyIndex(
    u32 queuesCount, VkPhysicalDevice device,
    const std::vector<VkQueueFamilyProperties>& propertiesVector) {
  for (i32 i = 0; i < queuesCount; i++) {
    VkQueueFlags queueFlags{ propertiesVector[i].queueFlags };

    b32 graphicsSupport{ supportsOperations<EQueueFamilyOperationsType::GRAPHICS>(queueFlags) };
    if (not graphicsSupport) {
      continue;
    }

    VkBool32 platformPresentationSupport{ supportsPlatformPresentation(device, i) };
    if (platformPresentationSupport) {
      return std::make_pair(i, propertiesVector[i]);
    }
  }

  return std::make_pair(VK_QUEUE_FAMILY_IGNORED, VkQueueFamilyProperties{});
}


VkBool32 supportsPlatformPresentation(VkPhysicalDevice physicalDevice, u32 familyIndex) {
  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    return vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, familyIndex);
  }
  else {
    return VK_FALSE;
  }
}


template<EQueueFamilyOperationsType TOperationsType>
b32 supportsOperations(VkQueueFlags queueFlags) {
  if constexpr (EQueueFamilyOperationsType::GRAPHICS == TOperationsType) {
    return (b32)(queueFlags & VK_QUEUE_GRAPHICS_BIT);
  }
  else if constexpr (EQueueFamilyOperationsType::COMPUTE == TOperationsType) {
    return (b32)(queueFlags & VK_QUEUE_COMPUTE_BIT);
  }
  else if constexpr (EQueueFamilyOperationsType::TRANSFER == TOperationsType) {
    return (b32)(queueFlags & VK_QUEUE_TRANSFER_BIT);
  }
  else if constexpr (EQueueFamilyOperationsType::SPARSE_BINDING == TOperationsType) {
    return (b32)(queueFlags & VK_QUEUE_SPARSE_BINDING_BIT);
  }
  else {
    return UFALSE;
  }
}


void displayPhysicalDeviceData(const VkPhysicalDeviceProperties& deviceProperties,
                               const VkPhysicalDeviceFeatures& deviceFeatures,
                               const VkQueueFamilyProperties& queueFamilyProperties,
                               u32 queueFamilyIndex) {
  UINFO("PhysicalDevice, picked: {}\nVulkan Version Available: {}.{}\nDriver Version: {}\nType: {}",
        deviceProperties.deviceName, VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_API_VERSION_MINOR(deviceProperties.apiVersion), deviceProperties.driverVersion,
        deviceProperties.deviceType);

  if (queueFamilyIndex == VK_QUEUE_FAMILY_IGNORED) {
    return;
  }
  auto queueFlags{ queueFamilyProperties.queueFlags };
  UINFO("QueueFamilyIndex {} supports check:\ngraphics: {}\ncompute: {}\ntransfer: "
        "{}\nsparse_binding: {}", queueFamilyIndex, queueFlags & VK_QUEUE_GRAPHICS_BIT,
        queueFlags & VK_QUEUE_COMPUTE_BIT, queueFlags & VK_QUEUE_TRANSFER_BIT,
        queueFlags & VK_QUEUE_SPARSE_BINDING_BIT);
}


}
