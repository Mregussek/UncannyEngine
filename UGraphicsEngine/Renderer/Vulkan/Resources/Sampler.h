
#ifndef UNCANNYENGINE_SAMPLER_H
#define UNCANNYENGINE_SAMPLER_H


#include <volk.h>


namespace uncanny::vulkan
{


class FSampler
{
public:

  FSampler() = default;
  FSampler(VkDevice vkDevice);
  ~FSampler();

  void Create();
  void Destroy();

  [[nodiscard]] VkSampler GetHandle() const { return m_Sampler; }

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkSampler m_Sampler{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SAMPLER_H
