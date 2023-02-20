
#ifndef UNCANNYENGINE_INSTANCE_H
#define UNCANNYENGINE_INSTANCE_H


#include <volk.h>
#include "InstanceProperties.h"


namespace uncanny::vulkan {


class FInstanceProperties;


/*
 * @brief FInstance is wrapper class for VkInstance functionality. App is responsible for destruction
 * of VkInstance with vkDestroyInstance. FInstanceProperties delivers all required layers and extensions along
 * with API version during creation.
 */
class FInstance {
public:

  void Create(const FInstanceProperties& properties);
  void Destroy();

  [[nodiscard]] VkInstance GetHandle() const { return m_Instance; }

private:

  FInstanceProperties m_Properties{};
  VkInstance m_Instance{ VK_NULL_HANDLE };

};


}

#endif //UNCANNYENGINE_INSTANCE_H
