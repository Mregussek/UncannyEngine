
#ifndef UNCANNYENGINE_EXTDEBUGUTILS_H
#define UNCANNYENGINE_EXTDEBUGUTILS_H


#include <volk.h>


namespace uncanny::vulkan
{


/// @brief FEXTDebugUtils is wrapper class for DebugUtilsMessenger handle in Vulkan. It enables validation
/// prints for Vulkan API.
/// @details Must be used only in RenderContext. RenderContext is responsible for its lifetime.
/// In this class I don't want to own VkInstance as it is not necessary. RenderContext should be able
/// to provide handle.
/// As DebugReport is deprecated, I have decided to comment it and not delete its implementation.
class FEXTDebugUtils
{
public:

  /// @brief Creates debug handle and initializes validation layers
  /// @param vkInstance VkInstance handle
  void Create(VkInstance vkInstance);

  /// @brief Destroys debug handle
  /// @param vkInstance VkInstance handle
  void Destroy(VkInstance vkInstance);

// Members
private:

  /// @brief debug handle
  VkDebugUtilsMessengerEXT m_DebugUtils{ VK_NULL_HANDLE };
  // VkDebugReportCallbackEXT m_DebugReport{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_EXTDEBUGUTILS_H
