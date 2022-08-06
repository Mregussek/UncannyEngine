
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanWindowSurface.h"
#include <utilities/Logger.h>


namespace uncanny
{


struct FSuitablePhysicalDeviceReturnInfo {
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  VkPhysicalDeviceFeatures physicalDeviceFeatures{};
  VkPhysicalDeviceProperties physicalDeviceProperties{};
  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesVector{};
  std::vector<u32> queueFamilyIndexVector{};
};


static b32 pickSuitableDevice(const FWindow* pWindow, VkInstance instance,
                              const FPhysicalDeviceDependencies& dependencies,
                              FSuitablePhysicalDeviceReturnInfo* pReturnInfo);


static b32 isProperDeviceType(EPhysicalDeviceType deviceType, VkPhysicalDeviceType vkDeviceType);


static b32 isCapableOfGraphicsOperations(u32 queueFamilyIndex, VkQueueFlags queueFlags,
                                         const FWindow* pWindow, VkInstance instance,
                                         VkPhysicalDevice device);


static b32 supportsPlatformPresentation(VkInstance instance, VkPhysicalDevice physicalDevice,
                                        u32 familyIndex);


static b32 supportsSwapchainExtension(VkPhysicalDevice physicalDevice);


static void displayPhysicalDeviceData(
    const VkPhysicalDeviceProperties& deviceProperties,
    const VkPhysicalDeviceFeatures& deviceFeatures,
    const std::vector<VkQueueFamilyProperties>& queueFamilyPropertiesVector,
    const std::vector<u32>& queueFamilyIndex);


b32 FRenderContextVulkan::createPhysicalDevice() {
  UTRACE("Creating Physical Device...");

  FSuitablePhysicalDeviceReturnInfo returnInfo{};
  b32 foundDevice{
    pickSuitableDevice(mSpecs.pWindow, mVkInstance, mPhysicalDeviceDependencies, &returnInfo) };
  if (not foundDevice) {
    UFATAL("Could not pick suitable VkPhysicalDevice! There is no integrated GPU nor discrete, "
           "there is no GPU supporting graphics nor platform presentation");
    return UFALSE;
  }

  UTRACE("Assigning return infos to class member variables...");
  mVkPhysicalDevice = returnInfo.physicalDevice;
  mVkPhysicalDeviceFeatures = returnInfo.physicalDeviceFeatures;
  mVkPhysicalDeviceProperties = returnInfo.physicalDeviceProperties;
  mQueueFamilyIndexVector = returnInfo.queueFamilyIndexVector;
  mVkQueueFamilyPropertiesVector = returnInfo.queueFamilyPropertiesVector;

  vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &mVkPhysicalDeviceMemoryProperties);

  displayPhysicalDeviceData(mVkPhysicalDeviceProperties, mVkPhysicalDeviceFeatures,
                            mVkQueueFamilyPropertiesVector, mQueueFamilyIndexVector);

  UDEBUG("Created Physical Device!");
  return UTRUE;
}


b32 FRenderContextVulkan::closePhysicalDevice() {
  UTRACE("Closing Physical Device...");

  if (mVkPhysicalDevice == VK_NULL_HANDLE) {
    UWARN("Physical device is not closed, as it wasn't created!");
    return UTRUE;
  }

  mVkPhysicalDevice = VK_NULL_HANDLE; // Physical device is destroyed along with VkInstance!

  UDEBUG("Closed Physical Device!");
  return UTRUE;
}


b32 pickSuitableDevice(const FWindow* pWindow, VkInstance instance,
                       const FPhysicalDeviceDependencies& dependencies,
                       FSuitablePhysicalDeviceReturnInfo* pReturnInfo) {
  UTRACE("Retrieving count and data about physical devices...");
  u32 devicesCount{ 0 };
  U_VK_ASSERT( vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr) );
  std::vector<VkPhysicalDevice> devicesVector(devicesCount);
  U_VK_ASSERT( vkEnumeratePhysicalDevices(instance, &devicesCount, devicesVector.data()) );

  UTRACE("Trying to find suitable vulkan physical device...");
  for (VkPhysicalDevice device : devicesVector) {
    // Retrieve device properties and features
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Retrieve queues family
    u32 queuesCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesCount, nullptr);
    std::vector<VkQueueFamilyProperties> queuePropertiesVector(queuesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queuesCount, queuePropertiesVector.data());

    UTRACE("Make sure that device is correct type");
    b32 mainTypeProper = isProperDeviceType(dependencies.deviceType, deviceProperties.deviceType);
    b32 fallbackTypeProper = isProperDeviceType(dependencies.deviceTypeFallback, deviceProperties.deviceType);
    if (not (mainTypeProper or fallbackTypeProper)) {
      UTRACE("Device {} isn't main nor fallback type", deviceProperties.deviceName);
      continue;
    }

    UTRACE("Make sure that device has swapchain support");
    if (not supportsSwapchainExtension(device)) {
      UTRACE("Device {} doesn't have swapchain support!", deviceProperties.deviceName);
      continue;
    }

    UTRACE("Make sure that device has proper count of queues family indexes");
    if (dependencies.queueFamilyIndexesCount > queuesCount) {
      UTRACE("Device {} doesn't have enough count of QueueFamilyIndexes", deviceProperties.deviceName);
      continue;
    }

    UTRACE("Make sure that proper queue family supports required operations");
    for (u32 i = 0; i < queuesCount; i++) {
      VkQueueFlags queueFlags{ queuePropertiesVector[i].queueFlags };
      u32 availableQueueCount{ queuePropertiesVector[i].queueCount };

      if (dependencies.queueFamilyDependencies[i].queuesCountNeeded > availableQueueCount) {
        UTRACE("Device {} queueFamilyIndex {} doesn't have enough count of Queues",
               deviceProperties.deviceName, i);
        continue;
      }
      if (dependencies.queueFamilyDependencies[i].graphics) {
        if (not isCapableOfGraphicsOperations(i, queueFlags, pWindow, instance, device)) {
          UTRACE("Device {} queueFamilyIndex {} doesn't support graphics",
                 deviceProperties.deviceName, i);
          continue;
        }
      }
      if (dependencies.queueFamilyDependencies[i].compute) {
        // TODO: implement compute capabilities check for queue family index
        UERROR("UncannyEngine TODO: implement compute capabilities check for queue family index");
        UTRACE("Device {} queueFamilyIndex {} doesn't support compute",
               deviceProperties.deviceName, i);
        continue;
      }
      if (dependencies.queueFamilyDependencies[i].transfer) {
        // TODO: implement transfer capabilities check for queue family index
        UERROR("UncannyEngine TODO: implement transfer capabilities check for queue family index");
        UTRACE("Device {} queueFamilyIndex {} doesn't support transfer",
               deviceProperties.deviceName, i);
        continue;
      }
      if (dependencies.queueFamilyDependencies[i].sparseBinding) {
        // TODO: implement sparseBinding capabilities check for queue family index
        UERROR("UncannyEngine TODO: implement sparseBinding capabilities check for queue family index");
        UTRACE("Device {} queueFamilyIndex {} doesn't support sparseBinding",
               deviceProperties.deviceName, i);
        continue;
      }

      // If everything is fine assign return values...
      UTRACE("Assigning queue family to return info...");
      pReturnInfo->queueFamilyIndexVector.push_back(i);
      pReturnInfo->queueFamilyPropertiesVector.push_back(queuePropertiesVector[i]);

      // If required of queue families indexes count is ready, break loop
      if (dependencies.queueFamilyIndexesCount - 1 == i) {
        UTRACE("As there is enough queue families indexes as in dependencies, breaking...");
        break;
      }
    }

    // If there are missing count of queue families, cannot use device
    if (dependencies.queueFamilyIndexesCount != pReturnInfo->queueFamilyIndexVector.size()) {
      UTRACE("Missing queue families indexes count, cannot use device {} != {}",
             dependencies.queueFamilyIndexesCount, pReturnInfo->queueFamilyIndexVector.size());
      continue;
    }

    // If everything is fine assign return values...
    UTRACE("Assigning physical device info to return info...");
    pReturnInfo->physicalDevice = device;
    pReturnInfo->physicalDeviceProperties = deviceProperties;
    pReturnInfo->physicalDeviceFeatures = deviceFeatures;
    return UTRUE;
  }

  UERROR("Could not find suitable vulkan physical device!");
  return UFALSE;
}


b32 isProperDeviceType(EPhysicalDeviceType deviceType, VkPhysicalDeviceType vkDeviceType) {
  if (
      deviceType == EPhysicalDeviceType::DISCRETE &&
      vkDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
      ) {
    UTRACE("Device is discrete type");
    return UTRUE;
  }
  else if (
      deviceType == EPhysicalDeviceType::INTEGRATED &&
      vkDeviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
      ) {
    UTRACE("Device is integrated type");
    return UTRUE;
  }

  UWARN("Device is unknown type! Vulkan Result: {}", vkDeviceType);
  return UFALSE;
}


b32 isCapableOfGraphicsOperations(u32 queueFamilyIndex, VkQueueFlags queueFlags,
                                  const FWindow* pWindow, VkInstance instance,
                                  VkPhysicalDevice device) {
  u32 graphicsSupport{ queueFlags & VK_QUEUE_GRAPHICS_BIT };
  if (not graphicsSupport) {
    UTRACE("Ignoring queue family index {} as it does not support graphics!", queueFamilyIndex);
    return UFALSE;
  }

  b32 presentationSupport{ supportsPlatformPresentation(instance, device, queueFamilyIndex) };
  if (not presentationSupport) {
    UTRACE("Ignoring queue family index {} as it does not support platform present!", queueFamilyIndex);
    return UFALSE;
  }

  b32 windowSurfacePresentationSupport{
      windowSurfaceSupportPresentationOnPhysicalDevice(pWindow, instance, device, queueFamilyIndex) };
  if (not windowSurfacePresentationSupport) {
    UTRACE("Ignoring queue family index {} as it does not support window surface present!", queueFamilyIndex);
    return UFALSE;
  }

  return UTRUE;
}


b32 supportsPlatformPresentation(VkInstance instance, VkPhysicalDevice physicalDevice,
                                 u32 familyIndex) {
  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    VkBool32 result{ vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, familyIndex) };
    if (result) {
      UTRACE("Presentation supported by Win32 Platform!");
      return UTRUE;
    }

    UERROR("Presentation is not supported by Win32 Platform!");
    return UFALSE;
  }
  else {
    return VK_FALSE;
  }
}


b32 supportsSwapchainExtension(VkPhysicalDevice physicalDevice) {
  u32 extensionCount{ 0 };
  U_VK_ASSERT( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                                    nullptr));
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  U_VK_ASSERT( vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                                    availableExtensions.data()) );

  auto it = std::find_if(availableExtensions.begin(), availableExtensions.end(),
                         [](VkExtensionProperties& properties) -> b32 {
    return std::strcmp(properties.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  });
  if (it == availableExtensions.end()) {
    UTRACE("Device does not support swapchain!");
    return UFALSE;
  }

  UTRACE("Device supports swapchain!");
  return UTRUE;
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
                               const std::vector<VkQueueFamilyProperties>& queueFamilyPropertiesVector,
                               const std::vector<u32>& queueFamilyIndexVector) {
  FDriverVersionInfo driverVersionInfo =
      decodeDriverVersionVulkan(deviceProperties.driverVersion, deviceProperties.vendorID);
  UINFO("PhysicalDevice Info\nPicked: {}\nDriver Version: {}.{}\nVulkan Version Available: {}"
        ".{}\nType: {}", deviceProperties.deviceName, driverVersionInfo.variant,
        driverVersionInfo.major, VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_API_VERSION_MINOR(deviceProperties.apiVersion),
        getGpuTypeName(deviceProperties.deviceType));

  for (u32 i = 0; i < queueFamilyIndexVector.size(); i++) {
    UINFO("QueueFamilyIndex {} supports check:\ngraphics: {}\ncompute: {}\ntransfer: "
          "{}\nsparse_binding: {}\nqueuesCountAvailable: {}",
          queueFamilyIndexVector[i],
          queueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
          queueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_COMPUTE_BIT,
          queueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_TRANSFER_BIT,
          queueFamilyPropertiesVector[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT,
          queueFamilyPropertiesVector[i].queueCount);
  }


}


}
