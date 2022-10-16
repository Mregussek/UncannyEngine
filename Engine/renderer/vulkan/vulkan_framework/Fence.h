
#ifndef UNCANNYENGINE_FENCE_H
#define UNCANNYENGINE_FENCE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf
{


struct FFenceInitDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
};


class FFenceVulkan {
public:

  b32 init(const FFenceInitDependenciesVulkan& deps);
  void terminate(VkDevice device);

  void wait(VkDevice device) const;

  [[nodiscard]] VkFence Handle() const { return m_VkFence; }

private:

  VkFence m_VkFence{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_FENCE_H
