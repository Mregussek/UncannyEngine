
#ifndef UNCANNYENGINE_INSTANCEATTRIBUTES_H
#define UNCANNYENGINE_INSTANCEATTRIBUTES_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FInstanceAttributes is a helper class for FInstance. It is responsible for storing all the
/// requested instance layers and extensions. Also it is able to validate if such layer and extension
/// is available on the GPU. Also It queries available API version that is ready to use.
class FInstanceAttributes
{
public:

  FInstanceAttributes() = default;

  FInstanceAttributes(const FInstanceAttributes& other) = default;
  FInstanceAttributes(FInstanceAttributes&& other) = delete;

  FInstanceAttributes& operator=(const FInstanceAttributes& other) = default;
  FInstanceAttributes& operator=(FInstanceAttributes&& other) = delete;

public:

  /// @brief Initializes attributes class with all the available layers, extensions and API version.
  void Initialize();

  /// @brief Adds requested layer if it is available
  /// @param layerName requested layer name, like "VK_LAYER_KHRONOS_validation"
  /// @returns boolean information, if requested layer is added properly
  b8 AddLayerName(const char* layerName);

  /// @brief Adds requested extension if it is available
  /// @param extensionName requested extension name, like VK_KHR_WIN32_SURFACE_EXTENSION_NAME
  /// @returns boolean information, if requested extension is added properly
  b8 AddExtensionName(const char* extensionName);

  /// @brief Checks if requested Vulkan API version is available
  /// @param apiVersion requested Vulkan API version, like VK_API_VERSION_1_3
  [[nodiscard]] b8 IsVersionAvailable(u32 apiVersion) const;

  /// @brief Getter method for supported version
  /// @returns supported Vulkan API version
  [[nodiscard]] u32 GetVersion() const { return m_SupportedVersion; }

  /// @brief Getter method for full supported version
  /// @details Version without variant and path, equal to VK_API_VERSION_1_3 for example
  /// @returns supported Vulkan API version
  [[nodiscard]] u32 GetFullVersion() const { return m_FullApiVersion; }

  /// @brief Checks whether given extension name is in requested ones
  /// @param extensionName extension to validate if it is requested by the user
  /// @returns boolean result, UTRUE if is passed extensionName is requested
  [[nodiscard]] b8 IsExtensionRequested(const char* extensionName) const;

// Getters
public:

  [[nodiscard]] const std::vector<const char*>& GetRequestedLayers() const { return m_RequestedLayers; }
  [[nodiscard]] const std::vector<const char*>& GetRequestedExtensions() const { return m_RequestedExtensions; }

private:

  void GatherAvailableVersion();
  void GatherAvailableLayers();
  void GatherAvailableExtensions();

  [[nodiscard]] b8 IsLayerAvailable(const char* layerName) const;
  [[nodiscard]] b8 IsExtensionAvailable(const char* extensionName) const;

private:

  std::vector<VkLayerProperties> m_AvailableLayerProperties{};
  std::vector<VkExtensionProperties> m_AvailableExtensionsProperties{};

  std::vector<const char*> m_RequestedLayers{};
  std::vector<const char*> m_RequestedExtensions{};

  // This version is the one retrieved from vkEnumerateInstanceVersion
  u32 m_SupportedVersion{ UVERSION_UNDEFINED };
  // This version is without patch and variant pats, only major na minor
  u32 m_FullApiVersion{ UVERSION_UNDEFINED };

};


}


#endif //UNCANNYENGINE_INSTANCEATTRIBUTES_H
