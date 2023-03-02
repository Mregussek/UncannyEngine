
#ifndef UNCANNYENGINE_PHYSICALDEVICE_H
#define UNCANNYENGINE_PHYSICALDEVICE_H


#include <volk.h>
#include "PhysicalDeviceAttributes.h"


namespace uncanny::vulkan
{


/*
 * @brief FPhysicalDevice is a wrapper class for VkPhysicalDevice handle. It must be used only by
 * Render Context. Initialize method is implemented because VkPhysicalDevice is rather "retrieved" from
 * VkInstance than created and then destroyed.
 */
class FPhysicalDevice
{
public:

  void Initialize(VkPhysicalDevice physicalDevice);

  [[nodiscard]] VkPhysicalDevice GetHandle() const { return m_PhysicalDevice; }
  [[nodiscard]] const FPhysicalDeviceAttributes& GetAttributes() const { return m_Attributes; }

private:

  FPhysicalDeviceAttributes m_Attributes{};
  VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICE_H
