
#ifndef UNCANNYENGINE_PHYSICALDEVICE_H
#define UNCANNYENGINE_PHYSICALDEVICE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf {


class FPhysicalDeviceVulkan {
public:

  b32 init();
  void terminate();

private:

  VkPhysicalDevice m_VkPhysicalDevice{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICE_H
