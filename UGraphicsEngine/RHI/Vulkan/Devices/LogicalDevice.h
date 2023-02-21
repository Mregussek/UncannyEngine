
#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include "LogicalDeviceAttributes.h"


namespace uncanny::vulkan {


class FLogicalDevice {
public:

  void Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice);
  void Destroy();

private:

  FLogicalDeviceAttributes m_Attributes{};
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICE_H
