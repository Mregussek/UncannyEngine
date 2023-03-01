
#ifndef UNCANNYENGINE_FENCE_H
#define UNCANNYENGINE_FENCE_H


#include <volk.h>


namespace uncanny::vulkan {


class FFence {
public:

  void Create(VkDevice vkDevice);
  void Destroy();

  void WaitAndReset() const;

private:

  VkFence m_Fence{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_FENCE_H
