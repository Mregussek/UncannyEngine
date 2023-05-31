
#ifndef UNCANNYENGINE_SAMPLER_H
#define UNCANNYENGINE_SAMPLER_H


#include <volk.h>


namespace uncanny::vulkan
{


class FSampler
{
public:

  ~FSampler();

  void Create(VkDevice vkDevice);
  void Destroy();

  [[nodiscard]] VkSampler GetHandle() const { return m_Sampler; }

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkSampler m_Sampler{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SAMPLER_H
