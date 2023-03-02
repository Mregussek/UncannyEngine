
#include "LogicalDeviceAttributes.h"
#include "PhysicalDeviceAttributes.h"
#include "QueueFamilySelector.h"


namespace uncanny::vulkan
{


void FLogicalDeviceAttributes::InitializeQueueFamilyIndexes(std::span<const VkQueueFamilyProperties> familyProperties,
                                                            VkInstance vkInstance,
                                                            VkPhysicalDevice vkPhysicalDevice)
{
  std::optional<FQueueFamilyIndex> graphicsQueueFamily =
      vulkan::FQueueFamilySelector().SelectGraphicsQueueFamily(familyProperties, vkInstance, vkPhysicalDevice);
  if (graphicsQueueFamily.has_value())
  {
    m_GraphicsQueueFamilyIndex = graphicsQueueFamily.value();
    m_GraphicsQueueIndex = 0;
  }

  std::optional<FQueueFamilyIndex> presentQueueFamily =
      vulkan::FQueueFamilySelector().SelectPresentQueueFamily(familyProperties, vkInstance, vkPhysicalDevice);
  if (presentQueueFamily.has_value())
  {
    m_PresentQueueFamilyIndex = presentQueueFamily.value();
    m_PresentQueueIndex = familyProperties[m_PresentQueueFamilyIndex].queueCount > 1 ? 1 : 0;
  }

  std::optional<FQueueFamilyIndex> transferQueueFamily =
      vulkan::FQueueFamilySelector().SelectTransferQueueFamily(familyProperties, vkInstance, vkPhysicalDevice);
  if (transferQueueFamily.has_value())
  {
    m_TransferQueueFamilyIndex = transferQueueFamily.value();
    m_TransferQueueIndex = 0;
  }

  std::optional<FQueueFamilyIndex> computeQueueFamily =
      vulkan::FQueueFamilySelector().SelectComputeQueueFamily(familyProperties, vkInstance, vkPhysicalDevice);
  if (computeQueueFamily.has_value())
  {
    m_ComputeQueueFamilyIndex = computeQueueFamily.value();
    m_ComputeQueueIndex = 0;
  }
}


void FLogicalDeviceAttributes::InitializeDeviceFeatures(const VkPhysicalDeviceFeatures& physicalDeviceFeatures)
{
  m_DeviceFeatures = physicalDeviceFeatures;
}


b8 FLogicalDeviceAttributes::AddExtensionName(const char* extensionName,
                                              const FPhysicalDeviceAttributes& physicalDeviceAttributes)
{
  if (physicalDeviceAttributes.IsExtensionPresent(extensionName))
  {
    m_RequestedExtensions.push_back(extensionName);
    return UTRUE;
  }

  return UFALSE;
}


}
