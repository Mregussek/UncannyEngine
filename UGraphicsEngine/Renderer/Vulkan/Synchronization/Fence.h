
#ifndef UNCANNYENGINE_FENCE_H
#define UNCANNYENGINE_FENCE_H


#include <volk.h>


namespace uncanny::vulkan
{


/// @brief FFence is a wrapper class for VkFence.
class FFence
{
public:

  FFence() = default;

  FFence(const FFence& other) = delete;
  FFence(FFence&& other) = default;

  FFence& operator=(const FFence& other) = delete;
  FFence& operator=(FFence&& other) = delete;

  ~FFence();

public:

  void Create(VkDevice vkDevice, VkFenceCreateFlags flags);
  void Destroy();

  void WaitAndReset() const;

// Getters
public:

  [[nodiscard]] VkFence GetHandle() const { return m_Fence; }

// Members
private:

  VkFence m_Fence{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_FENCE_H
