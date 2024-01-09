
#ifndef UNCANNYENGINE_SAMPLER_H
#define UNCANNYENGINE_SAMPLER_H


#include <volk.h>


namespace uncanny::vulkan
{


/// @brief FSampler is a wrapper for VkSampler.
class FSampler
{
public:

  FSampler() = default;

  FSampler(const FSampler& other) = delete;
  FSampler(FSampler&& other) = delete;

  FSampler& operator=(const FSampler& other) = delete;
  FSampler& operator=(FSampler&& other) = delete;

  ~FSampler();

public:

  void Create(VkDevice vkDevice);
  void Destroy();

// Getters
public:

  [[nodiscard]] VkSampler GetHandle() const { return m_Sampler; }

// Members
private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkSampler m_Sampler{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SAMPLER_H
