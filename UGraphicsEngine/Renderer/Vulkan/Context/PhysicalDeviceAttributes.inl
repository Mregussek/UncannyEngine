
#ifndef UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_INL
#define UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_INL


#include "PhysicalDeviceAttributes.h"


namespace uncanny::vulkan
{


  template<typename TProperties> void FPhysicalDeviceAttributes::QueryProperties2(TProperties* pProperties) const
  {
    VkPhysicalDeviceProperties2 deviceProperties2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext = static_cast<void*>(pProperties),
    };
    vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &deviceProperties2);
  }


  template<typename TFeatures> void FPhysicalDeviceAttributes::QueryFeatures2(TFeatures* pFeatures) const
  {
    VkPhysicalDeviceFeatures2 deviceFeatures2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = static_cast<void*>(pFeatures),
    };
    vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &deviceFeatures2);
  }


}


#endif //UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
