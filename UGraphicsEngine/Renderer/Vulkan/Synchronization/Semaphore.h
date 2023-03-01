
#ifndef UNCANNYENGINE_SEMAPHORE_H
#define UNCANNYENGINE_SEMAPHORE_H


#include <volk.h>


namespace uncanny::vulkan {


class FSemaphore {
public:

  void Create(VkDevice vkDevice);
  void Destroy();

  [[nodiscard]] VkSemaphore GetHandle() const { return m_Semaphore; }

private:

  VkSemaphore m_Semaphore{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SEMAPHORE_H
