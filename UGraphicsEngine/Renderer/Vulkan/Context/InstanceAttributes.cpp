
#include "InstanceAttributes.h"
#include <algorithm>
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


void FInstanceAttributes::Initialize()
{
  GatherAvailableVersion();
  GatherAvailableLayers();
  GatherAvailableExtensions();
}


b8 FInstanceAttributes::AddLayerName(const char* layerName)
{
  if (IsLayerAvailable(layerName))
  {
    m_RequestedLayers.push_back(layerName);
    return UTRUE;
  }

  return UFALSE;
}


b8 FInstanceAttributes::AddExtensionName(const char* extensionName)
{
  if (IsExtensionAvailable(extensionName))
  {
    m_RequestedExtensions.push_back(extensionName);
    return UTRUE;
  }

  return UFALSE;
}


b8 FInstanceAttributes::IsVersionAvailable(u32 apiVersion) const
{
  // Supported version has always higher value than default VK_API_VERSION_1_3 for example
  if (m_SupportedVersion >= apiVersion)
  {
    return UTRUE;
  }

  return UFALSE;
}


b8 FInstanceAttributes::IsExtensionRequested(const char* extensionName) const
{
  auto found = std::ranges::find_if(m_RequestedExtensions,
                                    [extensionName](const char* requestedExtension) -> b32
  {
    return std::strcmp(extensionName, requestedExtension) == 0;
  });
  if (found != m_RequestedExtensions.end())
  {
    return UTRUE;
  }
  return UFALSE;
}


void FInstanceAttributes::GatherAvailableVersion()
{
  VkResult result{ vkEnumerateInstanceVersion(&m_SupportedVersion) };
  AssertVkAndThrow(result);
}


void FInstanceAttributes::GatherAvailableLayers()
{
  u32 layerCount{ 0 };
  VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  AssertVkAndThrow(result);
  m_AvailableLayerProperties.resize(layerCount);
  result = vkEnumerateInstanceLayerProperties(&layerCount, m_AvailableLayerProperties.data());
  AssertVkAndThrow(result);
}


void FInstanceAttributes::GatherAvailableExtensions()
{
  u32 extensionCount{ 0 };
  VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  AssertVkAndThrow(result);
  m_AvailableExtensionsProperties.resize(extensionCount);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_AvailableExtensionsProperties.data());
  AssertVkAndThrow(result);
}


b8 FInstanceAttributes::IsLayerAvailable(const char* layerName) const
{
  auto foundLayer = std::ranges::find_if(m_AvailableLayerProperties,
                                         [layerName](const VkLayerProperties& vkLayerProperties) -> b32
  {
    return std::strcmp(layerName, vkLayerProperties.layerName) == 0;
  });
  if (foundLayer == m_AvailableLayerProperties.end())
  {
    UERROR("Layer {} is not available!", layerName);
    return UFALSE;
  }

  return UTRUE;
}


b8 FInstanceAttributes::IsExtensionAvailable(const char* extensionName) const
{
  auto foundExtension = std::ranges::find_if(m_AvailableExtensionsProperties,
                                             [extensionName](const VkExtensionProperties& vkExtensionProperties) -> b32
  {
    return std::strcmp(extensionName, vkExtensionProperties.extensionName) == 0;
  });
  if (foundExtension == m_AvailableExtensionsProperties.end())
  {
    UERROR("Extension {} is not available!", extensionName);
    return UFALSE;
  }

  return UTRUE;
}


}
