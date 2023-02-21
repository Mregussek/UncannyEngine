
#ifndef UNCANNYENGINE_COMMANDPOOL_H
#define UNCANNYENGINE_COMMANDPOOL_H


#include <volk.h>


namespace uncanny::vulkan {


class FCommandPool {
public:

  FCommandPool(VkDevice vkDevice, VkCommandPool vkCommandPool);

  void Destroy();

  void Reset();

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };


};


}


#endif //UNCANNYENGINE_COMMANDPOOL_H
