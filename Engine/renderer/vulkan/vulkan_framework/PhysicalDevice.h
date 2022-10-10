
#ifndef UNCANNYENGINE_PHYSICALDEVICE_H
#define UNCANNYENGINE_PHYSICALDEVICE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf {


struct FPhysicalDeviceInitDependenciesVulkan {
  VkInstance instance{ VK_NULL_HANDLE };
};


class FPhysicalDeviceVulkan {
public:

  b32 init(const FPhysicalDeviceInitDependenciesVulkan& deps);
  void terminate();

private:

  VkPhysicalDevice m_VkPhysicalDevice{ VK_NULL_HANDLE };
  u32 m_QueueFamilyGraphics{ UUNUSED };
  u32 m_QueueFamilyTransfer{ UUNUSED };

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICE_H
