
#include "LogicalDeviceAttributes.h"
#include "PhysicalDeviceAttributes.h"
#include "QueueFamilySelector.h"


namespace uncanny::vulkan {


void FLogicalDeviceAttributes::InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                            VkInstance vkInstance,
                                                            VkPhysicalDevice vkPhysicalDevice) {
  std::optional<FQueueFamilyIndex> graphicsQueueFamily =
      vulkan::FQueueFamilySelector().SelectGraphicsQueueFamily(queueFamilyProperties, vkInstance, vkPhysicalDevice);
  if (graphicsQueueFamily.has_value()) {
    m_GraphicsQueueFamilyIndex = graphicsQueueFamily.value();
    m_GraphicsQueueIndex = 0;
  }

  std::optional<FQueueFamilyIndex> presentQueueFamily =
      vulkan::FQueueFamilySelector().SelectPresentQueueFamily(queueFamilyProperties, vkInstance, vkPhysicalDevice);
  if (presentQueueFamily.has_value()) {
    m_PresentQueueFamilyIndex = presentQueueFamily.value();
    m_PresentQueueIndex = queueFamilyProperties[m_PresentQueueFamilyIndex].queueCount > 1 ? 1 : 0;
  }

  std::optional<FQueueFamilyIndex> transferQueueFamily =
      vulkan::FQueueFamilySelector().SelectTransferQueueFamily(queueFamilyProperties, vkInstance, vkPhysicalDevice);
  if (transferQueueFamily.has_value()) {
    m_TransferQueueFamilyIndex = transferQueueFamily.value();
    m_TransferQueueIndex = 0;
  }
}


void FLogicalDeviceAttributes::InitializeDeviceFeatures(const FPhysicalDeviceAttributes& physicalDeviceAttributes) {
  m_DeviceFeatures = physicalDeviceAttributes.GetDeviceFeatures();
}


b8 FLogicalDeviceAttributes::AddExtensionName(const char* extensionName,
                                              const FPhysicalDeviceAttributes& physicalDeviceAttributes) {
  if (physicalDeviceAttributes.IsExtensionPresent(extensionName)) {
    m_RequestedExtensions.push_back(extensionName);
    return UTRUE;
  }

  return UFALSE;
}


}
