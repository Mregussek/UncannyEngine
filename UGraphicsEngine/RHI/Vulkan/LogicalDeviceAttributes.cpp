
#include "LogicalDeviceAttributes.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "QueueFamilySelector.h"


namespace uncanny::vulkan {


void FLogicalDeviceAttributes::InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> queueFamilyProperties,
                                                            const FInstance& instance,
                                                            const FPhysicalDevice& physicalDevice) {
  std::optional<u32> graphicsQueueFamily = vulkan::FQueueFamilySelector().SelectGraphicsQueueFamily(queueFamilyProperties,
                                                                                                    instance,
                                                                                                    physicalDevice);
  if (graphicsQueueFamily.has_value()) {
    m_GraphicsQueueFamilyIndex = graphicsQueueFamily.value();
  }

  std::optional<u32> presentQueueFamily = vulkan::FQueueFamilySelector().SelectPresentQueueFamily(queueFamilyProperties,
                                                                                                  instance,
                                                                                                  physicalDevice);
  if (presentQueueFamily.has_value()) {
    m_PresentQueueFamilyIndex = presentQueueFamily.value();
  }

  std::optional<u32> transferQueueFamily = vulkan::FQueueFamilySelector().SelectTransferQueueFamily(queueFamilyProperties,
                                                                                                    instance,
                                                                                                    physicalDevice);
  if (transferQueueFamily.has_value()) {
    m_TransferQueueFamilyIndex = transferQueueFamily.value();
  }
}


}
