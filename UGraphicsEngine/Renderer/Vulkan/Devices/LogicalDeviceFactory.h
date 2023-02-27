
#ifndef UNCANNYENGINE_LOGICALDEVICEFACTORY_H
#define UNCANNYENGINE_LOGICALDEVICEFACTORY_H


#include "../Commands/CommandPool.h"
#include "LogicalDeviceAttributes.h"


namespace uncanny::vulkan {


class FLogicalDeviceFactory {

  friend class FLogicalDevice;

public:

  [[nodiscard]] FCommandPool CreateCommandPool(FQueueFamilyIndex queueFamilyIndex);

private:

  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICEFACTORY_H
