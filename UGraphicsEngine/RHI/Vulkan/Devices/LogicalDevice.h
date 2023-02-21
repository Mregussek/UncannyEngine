
#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include "LogicalDeviceAttributes.h"
#include "Queue.h"


namespace uncanny::vulkan {


class FLogicalDevice {
public:

  void Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice);
  void Destroy();

private:

  void InitializeQueues();


  FLogicalDeviceAttributes m_Attributes{};
  FQueue m_GraphicsQueue{};
  FQueue m_PresentQueue{};
  FQueue m_TransferQueue{};
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICE_H
