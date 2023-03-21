
#ifndef UNCANNYENGINE_INSTANCE_H
#define UNCANNYENGINE_INSTANCE_H


#include <volk.h>
#include "InstanceAttributes.h"


namespace uncanny::vulkan
{


/// @brief FInstance is wrapper class for VkInstance functionality. RenderContext controls lifetime.
/// @details This class should only create and destroy instance, FInstanceAttributes delivers all required
/// layers and extensions along with API version during creation.
/// Class should be used only inside RenderContext. No exposure to the end user.
class FInstance
{
public:

  /// @brief Creates Vulkan API instance handle
  /// @param attributes containing all required layers and extensions needed for startup for Vulkan API
  void Create(const FInstanceAttributes& attributes);

  /// @brief Destroys Vulkan API instance handle
  void Destroy();

  /// @brief Returns available physical devices, new compilers should do Return Value Optimization RVO
  /// Uses vkEnumeratePhysicalDevices to query this information.
  /// @return vector of available physical devices
  [[nodiscard]] std::vector<VkPhysicalDevice> QueryAvailablePhysicalDevices() const;

  [[nodiscard]] VkInstance GetHandle() const { return m_Instance; }

private:

  FInstanceAttributes m_Attributes{};
  VkInstance m_Instance{ VK_NULL_HANDLE };

};


}

#endif //UNCANNYENGINE_INSTANCE_H
