
#ifndef UNCANNYENGINE_QUEUEFAMILYSELECTOR_H
#define UNCANNYENGINE_QUEUEFAMILYSELECTOR_H


#ifdef WIN32
  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif
#include <volk.h>
#include <span>
#include <optional>
#include "Typedefs.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FQueueFamilySelector is a helper class for proper selection of every queue family needed in
/// framework.
/// @details Exposes Select() methods for every queue family. Only static methods implement to mark, that
/// class does not own anything.
class FQueueFamilySelector
{
public:

  /// @brief Calculates score for every properties and chooses best score for graphics queue family.
  /// @param properties all queue family properties
  /// @param vkInstance VkInstance handle (used for vkGetInstanceProcAddr)
  /// @param vkPhysicalDevice VkPhysicalDevice handle (used for support validation)
  /// @returns Optional value for queue family index as method may not find accurate index.
  [[nodiscard]] static std::optional<FQueueFamilyIndex> SelectGraphics(
      std::span<const VkQueueFamilyProperties> properties, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);

  /// @brief Calculates score for every properties and chooses best score for present queue family.
  /// @param properties all queue family properties
  /// @param vkInstance VkInstance handle (used for vkGetInstanceProcAddr)
  /// @param vkPhysicalDevice VkPhysicalDevice handle (used for support validation)
  /// @returns Optional value for queue family index as method may not find accurate index.
  [[nodiscard]] static std::optional<FQueueFamilyIndex> SelectPresent(
      std::span<const VkQueueFamilyProperties> properties, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);

  /// @brief Calculates score for every properties and chooses best score for transfer queue family.
  /// @param properties all queue family properties
  /// @param vkInstance VkInstance handle (used for vkGetInstanceProcAddr)
  /// @param vkPhysicalDevice VkPhysicalDevice handle (used for support validation)
  /// @returns Optional value for queue family index as method may not find accurate index.
  [[nodiscard]] static std::optional<FQueueFamilyIndex> SelectTransfer(
      std::span<const VkQueueFamilyProperties> properties, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);

  /// @brief Calculates score for every properties and chooses best score for compute queue family.
  /// @param properties all queue family properties
  /// @param vkInstance VkInstance handle (used for vkGetInstanceProcAddr)
  /// @param vkPhysicalDevice VkPhysicalDevice handle (used for support validation)
  /// @returns Optional value for queue family index as method may not find accurate index.
  [[nodiscard]] static std::optional<FQueueFamilyIndex> SelectCompute(
      std::span<const VkQueueFamilyProperties> properties, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice);

};


}


#endif //UNCANNYENGINE_QUEUEFAMILYSELECTOR_H
