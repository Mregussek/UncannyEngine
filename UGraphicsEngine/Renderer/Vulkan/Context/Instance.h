
#ifndef UNCANNYENGINE_INSTANCE_H
#define UNCANNYENGINE_INSTANCE_H


#include <volk.h>
#include "InstanceAttributes.h"


namespace uncanny::vulkan
{


/*
 * @brief FInstance is wrapper class for VkInstance functionality. RenderContext controls lifetime.
 * FInstanceProperties delivers all required layers and extensions along with API version during creation.
 * Class should be used only inside RenderContext. No exposure to the end user.
 */
class FInstance
{
public:

  void Create(const FInstanceAttributes& attributes);
  void Destroy();

  // Returning vector by value, but new compilers should do Return Value Optimization RVO
  [[nodiscard]] std::vector<VkPhysicalDevice> QueryAvailablePhysicalDevices() const;

  [[nodiscard]] VkInstance GetHandle() const { return m_Instance; }

private:

  FInstanceAttributes m_Attributes{};
  VkInstance m_Instance{ VK_NULL_HANDLE };

};


}

#endif //UNCANNYENGINE_INSTANCE_H
