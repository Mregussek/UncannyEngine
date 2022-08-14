
#include "RenderContextVulkan.h"
#include "VulkanUtilities.h"
#include "VulkanWindowSurface.h"
#include "VulkanImageDepth.h"
#include <utilities/Logger.h>


namespace uncanny
{


static b32 pickSuitablePhysicalDevice(
    const std::vector<VkPhysicalDevice>& physicalDevicesVector,
    const FPhysicalDeviceDependencies& dependencies,
    const FWindow* pWindow,
    VkInstance instance,
    VkPhysicalDevice* pOutPhysicalDevice);


static b32 isPhysicalDeviceSuitable(
    VkPhysicalDevice physicalDevice,
    const FPhysicalDeviceDependencies& dependencies,
    const FWindow* pWindow,
    VkInstance instance);


static b32 pickSuitableQueueFamily(
    const std::vector<VkQueueFamilyProperties>& queueFamilyPropertiesVector,
    const FQueueFamilyDependencies& dependencies,
    const FWindow* pWindow,
    VkInstance instance,
    VkPhysicalDevice physicalDevice,
    u32* pOutQueueFamilyIndex);


static b32 isQueueFamilySuitable(
    u32 queueFamilyIndex,
    const VkQueueFamilyProperties& properties,
    const FQueueFamilyDependencies& dependencies,
    const FWindow* pWindow,
    VkInstance instance,
    VkPhysicalDevice physicalDevice);


static b32 isPhysicalDeviceProperDeviceType(
    EPhysicalDeviceType deviceType,
    VkPhysicalDeviceType vkDeviceType);


static b32 isQueueFamilyCapableOfGraphicsOperations(
    u32 queueFamilyIndex,
    VkQueueFlags queueFlags,
    const FWindow* pWindow,
    VkInstance instance,
    VkPhysicalDevice device);


static b32 supportsPlatformPresentation(
    u32 queueFamilyIndex,
    VkInstance instance,
    VkPhysicalDevice physicalDevice);


static b32 supportsSwapchainExtension(VkPhysicalDevice physicalDevice);


static b32 granularityHasProperValuesForGraphics(VkExtent3D granularity);


static void displayPhysicalDeviceData(VkPhysicalDevice physicalDevice);


static void displayQueueFamilyData(
    u32 queueFamilyIndex,
    const std::vector<VkQueueFamilyProperties>& queueFamilyPropertiesVector);


b32 FRenderContextVulkan::createPhysicalDevice() {
  UTRACE("Creating Physical Device...");

  UTRACE("Retrieving count and data about physical devices...");
  u32 devicesCount{ 0 };
  U_VK_ASSERT( vkEnumeratePhysicalDevices(mVkInstance, &devicesCount, nullptr) );
  std::vector<VkPhysicalDevice> physicalDevicesVector(devicesCount);
  U_VK_ASSERT( vkEnumeratePhysicalDevices(mVkInstance, &devicesCount, physicalDevicesVector.data()) );

  b32 foundDevice = pickSuitablePhysicalDevice(physicalDevicesVector, mPhysicalDeviceDependencies,
                                               mSpecs.pWindow, mVkInstance, &mVkPhysicalDevice);
  if (not foundDevice) {
    UFATAL("Could not pick suitable VkPhysicalDevice! There is no integrated GPU nor discrete, "
           "there is no GPU supporting graphics nor platform presentation");
    return UFALSE;
  }
  displayPhysicalDeviceData(mVkPhysicalDevice);

  UTRACE("Retrieving count and data about queue families...");
  u32 queuesCount{ 0 };
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queuesCount, nullptr);
  mVkQueueFamilyProperties.resize(queuesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queuesCount,
                                           mVkQueueFamilyProperties.data());

  FQueueFamilyDependencies graphicsFamilyDependencies{
    getQueueFamilyDependencies(EQueueFamilyMainUsage::GRAPHICS,
                               mPhysicalDeviceDependencies.queueFamilyDependencies) };
  b32 foundQueueFamily = pickSuitableQueueFamily(mVkQueueFamilyProperties,
                                                 graphicsFamilyDependencies, mSpecs.pWindow,
                                                 mVkInstance, mVkPhysicalDevice,
                                                 &mGraphicsQueueFamilyIndex);
  if (not foundQueueFamily) {
    UFATAL("Could not pick suitable queue family index!");
    return UFALSE;
  }
  displayQueueFamilyData(mGraphicsQueueFamilyIndex, mVkQueueFamilyProperties);

  UDEBUG("Created Physical Device!");
  return UTRUE;
}


b32 FRenderContextVulkan::closePhysicalDevice() {
  UTRACE("Closing Physical Device...");

  if (mVkPhysicalDevice == VK_NULL_HANDLE) {
    UWARN("Physical device is not created, so it won't be closed!");
  }

  // Physical device is destroyed along with VkInstance!
  mVkPhysicalDevice = VK_NULL_HANDLE;

  UDEBUG("Closed Physical Device!");
  return UTRUE;
}


b32 pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevicesVector,
                               const FPhysicalDeviceDependencies& dependencies,
                               const FWindow* pWindow,
                               VkInstance instance,
                               VkPhysicalDevice* pOutPhysicalDevice) {
  UTRACE("Trying to find suitable vulkan physical device...");
  for (VkPhysicalDevice physicalDevice : physicalDevicesVector) {
    if (isPhysicalDeviceSuitable(physicalDevice, dependencies, pWindow, instance)) {
      UDEBUG("Found suitable physical device!");
      *pOutPhysicalDevice = physicalDevice;
      return UTRUE;
    }
  }

  UERROR("Could not find suitable vulkan physical device!");
  return UFALSE;
}


b32 isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice,
                             const FPhysicalDeviceDependencies& dependencies,
                             const FWindow* pWindow,
                             VkInstance instance) {
  // Retrieve device properties and features
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

  // Retrieve queues family
  u32 queuesCount{ 0 };
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queuesCount, nullptr);
  std::vector<VkQueueFamilyProperties> queuePropertiesVector(queuesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queuesCount,
                                           queuePropertiesVector.data());

  UTRACE("Make sure that device is correct type");
  b32 mainTypeProper =
      isPhysicalDeviceProperDeviceType(dependencies.deviceType, deviceProperties.deviceType);
  b32 fallbackTypeProper =
      isPhysicalDeviceProperDeviceType(dependencies.deviceTypeFallback, deviceProperties.deviceType);
  if (not (mainTypeProper or fallbackTypeProper)) {
    UTRACE("Device {} isn't main nor fallback type", deviceProperties.deviceName);
    return UFALSE;
  }

  UTRACE("Make sure that device has swapchain support");
  if (not supportsSwapchainExtension(physicalDevice)) {
    UTRACE("Device {} doesn't have swapchain support!", deviceProperties.deviceName);
    return UFALSE;
  }

  UTRACE("Make sure that device has depth format support");
  VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
  if (not detectSupportedDepthFormat(physicalDevice, dependencies.depthFormatDependencies,
                                     &depthFormat)) {
    UTRACE("Device {} doesn't support depth format candidates", deviceProperties.deviceName);
    return UFALSE;
  }

  UTRACE("Make sure that device has proper count of queues family indexes");
  if (dependencies.queueFamilyIndexesCount > queuesCount) {
    UTRACE("Device {} doesn't have enough count of QueueFamilyIndexes", deviceProperties.deviceName);
    return UFALSE;
  }

  UTRACE("Make sure that proper queue family supports required operations");
  for (u32 i = 0; i < dependencies.queueFamilyIndexesCount; i++) {
    u32 queueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
    b32 foundProperQueueFamily =
        pickSuitableQueueFamily(queuePropertiesVector, dependencies.queueFamilyDependencies[i],
                                pWindow, instance, physicalDevice, &queueFamilyIndex);
    if (not foundProperQueueFamily) {
      UTRACE("Device {} doesn't support expected queue family!", deviceProperties.deviceName);
      return UFALSE;
    }
  }

  // If everything is fine assign return values...
  UTRACE("Found physical device suitable...");
  return UTRUE;
}


b32 pickSuitableQueueFamily(const std::vector<VkQueueFamilyProperties>& queueFamilyPropertiesVector,
                            const FQueueFamilyDependencies& dependencies,
                            const FWindow* pWindow,
                            VkInstance instance,
                            VkPhysicalDevice physicalDevice,
                            u32* pOutQueueFamilyIndex) {
  for (u32 i = 0; i < queueFamilyPropertiesVector.size(); i++) {
    if (isQueueFamilySuitable(i, queueFamilyPropertiesVector[i], dependencies, pWindow, instance,
                              physicalDevice)) {
      UTRACE("Assigning output queue family index {}!", i);
      *pOutQueueFamilyIndex = i;
      return UTRUE;
    }
  }

  UERROR("Could not find suitable queue family index!");
  return UFALSE;
}


b32 isQueueFamilySuitable(u32 queueFamilyIndex,
                          const VkQueueFamilyProperties& properties,
                          const FQueueFamilyDependencies& dependencies,
                          const FWindow* pWindow,
                          VkInstance instance,
                          VkPhysicalDevice physicalDevice) {
  VkQueueFlags queueFlags{ properties.queueFlags };
  u32 availableQueueCount{ properties.queueCount };

  if (dependencies.queuesCountNeeded > availableQueueCount) {
    UTRACE("Device queueFamilyIndex {} doesn't have enough count of Queues", queueFamilyIndex);
    return UFALSE;
  }

  // If graphics support is required...
  if (dependencies.graphics) {
    // check if there is graphics support
    if (not isQueueFamilyCapableOfGraphicsOperations(queueFamilyIndex, queueFlags, pWindow,
                                                     instance, physicalDevice)) {
      UTRACE("Device queueFamilyIndex {} doesn't support graphics", queueFamilyIndex);
      return UFALSE;
    }

    // graphics family should have no additional restrictions on the granularity of image
    // transfer operations
    if (not granularityHasProperValuesForGraphics(properties.minImageTransferGranularity)) {
      UTRACE("Device queueFamilyIndex {} has wrong minImageTransferGranularity for graphics. "
             "Should be (1, 1, 1)", queueFamilyIndex);
      return UFALSE;
    }
  }
  if (dependencies.compute) {
    // TODO: implement compute capabilities check for queue family index
    UERROR("UncannyEngine TODO: implement compute capabilities check for queue family index");
    UTRACE("Device queueFamilyIndex {} doesn't support compute", queueFamilyIndex);
    return UFALSE;
  }
  if (dependencies.transfer) {
    // TODO: implement transfer capabilities check for queue family index
    UERROR("UncannyEngine TODO: implement transfer capabilities check for queue family index");
    UTRACE("Device queueFamilyIndex {} doesn't support transfer", queueFamilyIndex);
    return UFALSE;
  }
  if (dependencies.sparseBinding) {
    // TODO: implement sparseBinding capabilities check for queue family index
    UERROR("UncannyEngine TODO: implement sparseBinding capabilities check for queue family index");
    UTRACE("Device queueFamilyIndex {} doesn't support sparseBinding", queueFamilyIndex);
    return UFALSE;
  }

  UTRACE("Found suitable queue family index {}!", queueFamilyIndex);
  return UTRUE;
}


b32 isPhysicalDeviceProperDeviceType(EPhysicalDeviceType deviceType,
                                     VkPhysicalDeviceType vkDeviceType) {
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

  UWARN("Device is unknown type! UExpected: {} Vulkan Result: {}", (i32)deviceType, vkDeviceType);
  return UFALSE;
}


b32 isQueueFamilyCapableOfGraphicsOperations(u32 queueFamilyIndex, VkQueueFlags queueFlags,
                                             const FWindow* pWindow, VkInstance instance,
                                             VkPhysicalDevice device) {
  u32 graphicsSupport{ queueFlags & VK_QUEUE_GRAPHICS_BIT };
  if (not graphicsSupport) {
    UTRACE("Ignoring queue family index {} as it does not support graphics!", queueFamilyIndex);
    return UFALSE;
  }

  b32 presentationSupport{ supportsPlatformPresentation(queueFamilyIndex, instance, device) };
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


b32 supportsPlatformPresentation(u32 queueFamilyIndex,
                                 VkInstance instance,
                                 VkPhysicalDevice physicalDevice) {
  if constexpr (VK_USE_PLATFORM_WIN32_KHR) {
    auto vkGetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    VkBool32 result{ vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice,
                                                                    queueFamilyIndex) };
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


b32 granularityHasProperValuesForGraphics(VkExtent3D granularity) {
  if (granularity.width == 1 and granularity.height == 1 and granularity.depth == 1) {
    UTRACE("Min Image Transfer Granularity has correct values ({}, {}, {})!", granularity.width,
           granularity.height, granularity.depth);
    return UTRUE;
  }

  UERROR("Min Image Transfer Granularity has wrong values ({}, {}, {})!", granularity.width,
         granularity.height, granularity.depth);
  return UFALSE;
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


void displayPhysicalDeviceData(VkPhysicalDevice physicalDevice) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

  FDriverVersionInfo driverVersionInfo =
      decodeDriverVersionVulkan(deviceProperties.driverVersion, deviceProperties.vendorID);

  UINFO("PhysicalDevice Info\nPicked: {}\nDriver Version: {}.{}.{}\nVulkan Version Available: {}"
        ".{}\nType: {}", deviceProperties.deviceName, driverVersionInfo.major,
        driverVersionInfo.minor,  driverVersionInfo.patch,
        VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_API_VERSION_MINOR(deviceProperties.apiVersion),
        getGpuTypeName(deviceProperties.deviceType));
}


void displayQueueFamilyData(
    u32 queueFamilyIndex,
    const std::vector<VkQueueFamilyProperties>& queueFamilyPropertiesVector) {
  UINFO("QueueFamilyIndex {} supports check:\ngraphics: {}\ncompute: {}\ntransfer: "
        "{}\nsparse_binding: {}\nqueuesCountAvailable: {}",
        queueFamilyIndex,
        queueFamilyPropertiesVector[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT,
        queueFamilyPropertiesVector[queueFamilyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT,
        queueFamilyPropertiesVector[queueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT,
        queueFamilyPropertiesVector[queueFamilyIndex].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT,
        queueFamilyPropertiesVector[queueFamilyIndex].queueCount);
}


}
