
#ifndef UNCANNYENGINE_COMMANDPOOL_H
#define UNCANNYENGINE_COMMANDPOOL_H


#include <volk.h>
#include "CommandPoolFactory.h"


namespace uncanny::vulkan {


class FCommandPool {
public:

  FCommandPool() = delete;
  FCommandPool(VkDevice vkDevice, VkCommandPool vkCommandPool);

  void Destroy();

  void Reset();

  [[nodiscard]] const FCommandPoolFactory& GetFactory() const { return m_Factory; }

private:

  FCommandPoolFactory m_Factory{};
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };


};


}


#endif //UNCANNYENGINE_COMMANDPOOL_H
