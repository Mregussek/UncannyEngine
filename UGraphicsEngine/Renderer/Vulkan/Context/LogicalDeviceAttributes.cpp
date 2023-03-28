
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
      vulkan::FQueueFamilySelector::SelectGraphics(familyProperties, vkInstance, vkPhysicalDevice);
  if (graphicsQueueFamily.has_value())
  {
    m_GraphicsQueueFamilyIndex = graphicsQueueFamily.value();
    m_GraphicsQueueIndex = 0;
  }

  std::optional<FQueueFamilyIndex> presentQueueFamily =
      vulkan::FQueueFamilySelector::SelectPresent(familyProperties, vkInstance, vkPhysicalDevice);
  if (presentQueueFamily.has_value())
  {
    m_PresentQueueFamilyIndex = presentQueueFamily.value();
    m_PresentQueueIndex = familyProperties[m_PresentQueueFamilyIndex].queueCount > 1 ? 1 : 0;
  }

  std::optional<FQueueFamilyIndex> transferQueueFamily =
      vulkan::FQueueFamilySelector::SelectTransfer(familyProperties, vkInstance, vkPhysicalDevice);
  if (transferQueueFamily.has_value())
  {
    m_TransferQueueFamilyIndex = transferQueueFamily.value();
    m_TransferQueueIndex = 0;
  }

  std::optional<FQueueFamilyIndex> computeQueueFamily =
      vulkan::FQueueFamilySelector::SelectCompute(familyProperties, vkInstance, vkPhysicalDevice);
  if (computeQueueFamily.has_value())
  {
    m_ComputeQueueFamilyIndex = computeQueueFamily.value();
    m_ComputeQueueIndex = 0;
  }
}


void FLogicalDeviceAttributes::InitializeDeviceFeatures(const FPhysicalDeviceAttributes& physicalDeviceAttributes)
{
  m_DeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  m_DeviceFeatures2.features = physicalDeviceAttributes.GetDeviceFeatures();

  m_Vulkan11Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
  physicalDeviceAttributes.QueryFeatures2(&m_Vulkan11Features);
  m_DeviceFeatures2.pNext = &m_Vulkan11Features;

  m_Vulkan12Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
  physicalDeviceAttributes.QueryFeatures2(&m_Vulkan12Features);
  m_Vulkan11Features.pNext = &m_Vulkan12Features;

  m_Vulkan13Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
  physicalDeviceAttributes.QueryFeatures2(&m_Vulkan13Features);
  m_Vulkan12Features.pNext = &m_Vulkan13Features;

  void** pFeaturesChain{ &m_Vulkan13Features.pNext };

  if (physicalDeviceAttributes.IsExtensionPresent(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
  {
    m_RayTracingPipelineFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
    physicalDeviceAttributes.QueryFeatures2(&m_RayTracingPipelineFeatures);
    *pFeaturesChain = &m_RayTracingPipelineFeatures;
    pFeaturesChain = &m_RayTracingPipelineFeatures.pNext;
  }
  if (physicalDeviceAttributes.IsExtensionPresent(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
  {
    m_AccelerationStructureFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
    physicalDeviceAttributes.QueryFeatures2(&m_AccelerationStructureFeatures);
    *pFeaturesChain = &m_AccelerationStructureFeatures;
    pFeaturesChain = &m_AccelerationStructureFeatures.pNext;
  }
  if (physicalDeviceAttributes.IsExtensionPresent(VK_KHR_RAY_QUERY_EXTENSION_NAME))
  {
    m_RayQueryFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR };
    physicalDeviceAttributes.QueryFeatures2(&m_RayQueryFeatures);
    *pFeaturesChain = &m_RayQueryFeatures;
    pFeaturesChain = &m_RayQueryFeatures.pNext;
  }
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
