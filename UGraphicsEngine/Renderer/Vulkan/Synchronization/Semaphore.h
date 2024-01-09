
#ifndef UNCANNYENGINE_SEMAPHORE_H
#define UNCANNYENGINE_SEMAPHORE_H


#include <volk.h>


namespace uncanny::vulkan
{


/// @brief FSemaphore is a wrapper for VkSemaphore.
class FSemaphore
{
public:

  FSemaphore() = default;

  FSemaphore(const FSemaphore& other) = delete;
  FSemaphore(FSemaphore&& other) = default;

  FSemaphore& operator=(const FSemaphore& other) = delete;
  FSemaphore& operator=(FSemaphore&& other) = delete;

  ~FSemaphore();

public:

  void Create(VkDevice vkDevice);
  void Destroy();

// Getters
public:

  [[nodiscard]] VkSemaphore GetHandle() const { return m_Semaphore; }

// Members
private:

  VkSemaphore m_Semaphore{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_SEMAPHORE_H
