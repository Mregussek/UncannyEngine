
#include "InstanceProperties.h"
#include "Utilities.h"

#include <algorithm>

#include "UTools/Logger/Log.h"


namespace uncanny::vulkan {


void FInstanceProperties::Initialize() {
  GatherAvailableVersion();
  GatherAvailableLayers();
  GatherAvailableExtensions();
}


b8 FInstanceProperties::AddLayerName(const char* layerName) {
  if (IsLayerAvailable(layerName)) {
    m_RequestedLayers.push_back(layerName);
    return UTRUE;
  }

  return UFALSE;
}


b8 FInstanceProperties::AddExtensionName(const char* extensionName) {
  if (IsExtensionAvailable(extensionName)) {
    m_RequestExtensions.push_back(extensionName);
    return UTRUE;
  }

  return UFALSE;
}


b8 FInstanceProperties::IsVersionAvailable(u32 apiVersion) const {
  // Supported version has always higher value than default VK_API_VERSION_1_3 for example
  if (m_SupportedVersion >= apiVersion) {
    return UTRUE;
  }

  return UFALSE;
}


void FInstanceProperties::GatherAvailableVersion() {
  VkResult result{ vkEnumerateInstanceVersion(&m_SupportedVersion) };
  AssertVkAndThrow(result);
}


void FInstanceProperties::GatherAvailableLayers() {
  u32 layerCount{ 0 };
  VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  AssertVkAndThrow(result);
  m_AvailableLayerProperties.resize(layerCount);
  result = vkEnumerateInstanceLayerProperties(&layerCount,
                                              m_AvailableLayerProperties.data());
  AssertVkAndThrow(result);
}


void FInstanceProperties::GatherAvailableExtensions() {
  u32 extensionCount{ 0 };
  VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                           nullptr);
  AssertVkAndThrow(result);
  m_AvailableExtensionsProperties.resize(extensionCount);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                  m_AvailableExtensionsProperties.data());
  AssertVkAndThrow(result);
}


b8 FInstanceProperties::IsLayerAvailable(const char* layerName) const {
  auto foundLayer = std::ranges::find_if(m_AvailableLayerProperties, [layerName](const VkLayerProperties& vkLayerProperties) -> b32 {
    return layerName == std::string_view(vkLayerProperties.layerName);
  });
  if (foundLayer == m_AvailableLayerProperties.end()) {
    FLog::error("Layer {} is not available!", layerName);
    return UFALSE;
  }

  return UTRUE;
}


b8 FInstanceProperties::IsExtensionAvailable(const char* extensionName) const {
  auto foundExtension = std::ranges::find_if(m_AvailableExtensionsProperties, [extensionName](const VkExtensionProperties& vkExtensionProperties) -> b32 {
    return extensionName == std::string_view(vkExtensionProperties.extensionName);
  });
  if (foundExtension == m_AvailableExtensionsProperties.end()) {
    FLog::error("Extension {} is not available!", extensionName);
    return UFALSE;
  }

  return UTRUE;
}


}
