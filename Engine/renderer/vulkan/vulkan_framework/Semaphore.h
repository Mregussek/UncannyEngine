
#ifndef UNCANNYENGINE_SEMAPHORE_H
#define UNCANNYENGINE_SEMAPHORE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf
{


struct FSemaphoreInitDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
};


class FSemaphoreVulkan {
public:

  b32 init(const FSemaphoreInitDependenciesVulkan& deps);
  void terminate(VkDevice device);

  [[nodiscard]] VkSemaphore Handle() const { return m_VkSemaphore; }
  [[nodiscard]] VkSemaphore* HandlePtr() { return &m_VkSemaphore; }

private:

  VkSemaphore m_VkSemaphore{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SEMAPHORE_H
