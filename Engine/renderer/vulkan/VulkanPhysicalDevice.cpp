
#include "RenderContextVulkan.h"
#include <utilities/Logger.h>
#include "VulkanUtilities.h"


namespace uncanny
{


struct FSuitableQueueFamilyReturnInfo {
  VkQueueFamilyProperties queueFamilyProperties{};
  u32 queueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
};


struct FSuitablePhysicalDeviceReturnInfo {
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  FSuitableQueueFamilyReturnInfo queueFamilyReturnInfo{};
};


static b32 pickSuitableDevice(VkInstance instance, const std::vector<VkPhysicalDevice>& devicesVector,
                              FSuitablePhysicalDeviceReturnInfo* pReturnInfo);


static b32 pickSuitableQueueFamilyIndex(VkInstance instance, VkPhysicalDevice physicalDevice,
                                        const std::vector<VkQueueFamilyProperties>& queuePropertiesVector,
                                        u32 queuesCount, FSuitableQueueFamilyReturnInfo* pReturnInfo);


static VkBool32 supportsPlatformPresentation(VkInstance instance, VkPhysicalDevice physicalDevice,
                                             u32 familyIndex);


static void displayPhysicalDeviceData(const VkPhysicalDeviceProperties& deviceProperties,
                                      const VkPhysicalDeviceFeatures& deviceFeatures,
                                      const VkQueueFamilyProperties& queueFamilyProperties,
                                      u32 queueFamilyIndex);


b32 FRenderContextVulkan::createPhysicalDevice() {
  UTRACE("Creating Physical Device...");

  UTRACE("Retrieving count and data about physical devices...");
  u32 devicesCount{ 0 };
  U_VK_ASSERT( vkEnumeratePhysicalDevices(mInstanceVk, &devicesCount, nullptr) );
  std::vector<VkPhysicalDevice> devicesVector(devicesCount);
  U_VK_ASSERT( vkEnumeratePhysicalDevices(mInstanceVk, &devicesCount, devicesVector.data()) );

  FSuitablePhysicalDeviceReturnInfo returnInfo{};
  b32 foundDevice{ pickSuitableDevice(mInstanceVk, devicesVector, &returnInfo) };
  if (not foundDevice) {
    UFATAL("Could not pick suitable VkPhysicalDevice! There is no integrated GPU nor discrete, "
           "there is no GPU supporting graphics nor platform presentation");
    return UFALSE;
  }

  UTRACE("Assigning return infos to class member variables...");
  mPhysicalDeviceVk = returnInfo.physicalDevice;
  mQueueFamilyIndex = returnInfo.queueFamilyReturnInfo.queueFamilyIndex;
  mQueueFamilyProperties = returnInfo.queueFamilyReturnInfo.queueFamilyProperties;

  UTRACE("Saving physical device properties and features to member variables...");
  vkGetPhysicalDeviceProperties(mPhysicalDeviceVk, &mPhysicalDeviceProperties);
  vkGetPhysicalDeviceFeatures(mPhysicalDeviceVk, &mPhysicalDeviceFeatures);

  displayPhysicalDeviceData(mPhysicalDeviceProperties, mPhysicalDeviceFeatures,
                            mQueueFamilyProperties, mQueueFamilyIndex);

  UDEBUG("Created Physical Device!");
  return UTRUE;
}


b32 pickSuitableDevice(VkInstance instance, const std::vector<VkPhysicalDevice>& devicesVector,
                       FSuitablePhysicalDeviceReturnInfo* pReturnInfo) {
  UTRACE("Trying to find suitable vulkan physical device...");

  for (VkPhysicalDevice device : devicesVector) {
    // Retrieve device properties and features
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Make sure that device is GPU (integrated or discrete)
    b32 isIntegratedGPU{ deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU };
    b32 isDiscreteGPU{ deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU };
    if (not (isIntegratedGPU or isDiscreteGPU)) {
      continue;
    }

    // TODO: add VkPhysicalDeviceProperties.limits check
    // properties.limits ---> VkPhysicalDeviceLimits (check this for some parameters)

    // TODO: add VkPhysicalDeviceFeatures check
    // deviceFeatures ---> (check this for some shaders support, draw support etc.)

    // Retrieve queues family
    u32 queuesCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesCount, nullptr);
    std::vector<VkQueueFamilyProperties> queuePropertiesVector(queuesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesCount, queuePropertiesVector.data());

    // Make sure that proper queue family supports graphics operations
    FSuitableQueueFamilyReturnInfo queueReturnInfo{};
    b32 foundQueueFamily{ pickSuitableQueueFamilyIndex(instance, device, queuePropertiesVector,
                                                       queuesCount, &queueReturnInfo) };
    if (not foundQueueFamily) {
      continue;
    }

    // If everything is fine assign return values...
    UTRACE("Assigning physical device info to return info...");
    pReturnInfo->physicalDevice = device;
    pReturnInfo->queueFamilyReturnInfo = queueReturnInfo;
    return UTRUE;
  }

  UERROR("Could not find suitable vulkan physical device!");
  return UFALSE;
}


b32 pickSuitableQueueFamilyIndex(VkInstance instance, VkPhysicalDevice physicalDevice,
                                 const std::vector<VkQueueFamilyProperties>& queuePropertiesVector,
                                 u32 queuesCount, FSuitableQueueFamilyReturnInfo* pReturnInfo) {
  UTRACE("Trying to pick suitable queue family index...");

  for (u32 i = 0; i < queuesCount; i++) {
    VkQueueFlags queueFlags{ queuePropertiesVector[i].queueFlags };
    u32 graphicsSupport{ queueFlags & VK_QUEUE_GRAPHICS_BIT };
    if (not graphicsSupport) {
      continue;
    }

    VkBool32 presentationSupport{ supportsPlatformPresentation(instance, physicalDevice, i) };
    if (not presentationSupport) {
      continue;
    }

    // If everything is fine assign return values...
    UTRACE("Assigning queue family to return info...");
    pReturnInfo->queueFamilyIndex = i;
    pReturnInfo->queueFamilyProperties = queuePropertiesVector[i];
    return UTRUE;
  }

  UERROR("Could not find suitable queues family index!");
  return UFALSE;
}


VkBool32 supportsPlatformPresentation(VkInstance instance, VkPhysicalDevice physicalDevice,
                                      u32 familyIndex) {
  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    return vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, familyIndex);
  }
  else {
    return VK_FALSE;
  }
}


static const char* getGpuTypeName(VkPhysicalDeviceType type) {
  if (type == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
    return "Integrated";
  }
  else if (type == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    return "Discrete";
  }
  else {
    return "Unknown";
  }
}


void displayPhysicalDeviceData(const VkPhysicalDeviceProperties& deviceProperties,
                               const VkPhysicalDeviceFeatures& deviceFeatures,
                               const VkQueueFamilyProperties& queueFamilyProperties,
                               u32 queueFamilyIndex) {
  UINFO("PhysicalDevice Info\nPicked: {}\nDriver Version: {}\nVulkan Version Available: {}"
        ".{}\nType: {}", deviceProperties.deviceName, deviceProperties.driverVersion,
        VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_API_VERSION_MINOR(deviceProperties.apiVersion),
        getGpuTypeName(deviceProperties.deviceType));

  auto queueFlags{ queueFamilyProperties.queueFlags };
  UINFO("QueueFamilyIndex {} supports check:\ngraphics: {}\ncompute: {}\ntransfer: "
        "{}\nsparse_binding: {}", queueFamilyIndex, queueFlags & VK_QUEUE_GRAPHICS_BIT,
        queueFlags & VK_QUEUE_COMPUTE_BIT, queueFlags & VK_QUEUE_TRANSFER_BIT,
        queueFlags & VK_QUEUE_SPARSE_BINDING_BIT);
}


}
