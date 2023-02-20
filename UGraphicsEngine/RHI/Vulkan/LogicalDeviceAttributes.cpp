
#include "LogicalDeviceAttributes.h"
#include "PhysicalDeviceAttributes.h"
#include "QueueFamilySelector.h"


namespace uncanny::vulkan {


void FLogicalDeviceAttributes::InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                            VkInstance vkInstance,
                                                            VkPhysicalDevice vkPhysicalDevice) {
  std::optional<u32> graphicsQueueFamily = vulkan::FQueueFamilySelector().SelectGraphicsQueueFamily(queueFamilyProperties,
                                                                                                    vkInstance,
                                                                                                    vkPhysicalDevice);
  if (graphicsQueueFamily.has_value()) {
    m_GraphicsQueueFamilyIndex = graphicsQueueFamily.value();
  }

  std::optional<u32> presentQueueFamily = vulkan::FQueueFamilySelector().SelectPresentQueueFamily(queueFamilyProperties,
                                                                                                  vkInstance,
                                                                                                  vkPhysicalDevice);
  if (presentQueueFamily.has_value()) {
    m_PresentQueueFamilyIndex = presentQueueFamily.value();
  }

  std::optional<u32> transferQueueFamily = vulkan::FQueueFamilySelector().SelectTransferQueueFamily(queueFamilyProperties,
                                                                                                    vkInstance,
                                                                                                    vkPhysicalDevice);
  if (transferQueueFamily.has_value()) {
    m_TransferQueueFamilyIndex = transferQueueFamily.value();
  }
}


b8 FLogicalDeviceAttributes::AddExtensionName(const char* extensionName, const FPhysicalDeviceAttributes& physicalDeviceAttributes) {
  if (physicalDeviceAttributes.IsExtensionPresent(extensionName)) {
    m_RequestedExtensions.push_back(extensionName);
    return UTRUE;
  }

  return UFALSE;
}


}
