
#ifndef UNCANNYENGINE_INSTANCE_H
#define UNCANNYENGINE_INSTANCE_H


#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include "InstanceAttributes.h"


namespace uncanny::vulkan {


/*
 * @brief FInstance is wrapper class for VkInstance functionality. App is responsible for destruction
 * of VkInstance with vkDestroyInstance. FInstanceProperties delivers all required layers and extensions along
 * with API version during creation.
 * Class should be used only inside RHI, so that it will be used properly. No exposure to the end user.
 */
class FInstance {
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
