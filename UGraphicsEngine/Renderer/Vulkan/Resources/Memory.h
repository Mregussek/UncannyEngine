
#ifndef UNCANNYENGINE_MEMORY_H
#define UNCANNYENGINE_MEMORY_H


#include <volk.h>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FMemory is a wrapper around VkDeviceMemory.
/// Memory should not be exposed to the end user, FBuffer and FImage should be owners.
class FMemory
{
public:

  FMemory() = default;

  FMemory(const FMemory& other) = delete;
  FMemory(FMemory&& other) = default;

  FMemory& operator=(const FMemory& other) = delete;
  FMemory& operator=(FMemory&& other) = delete;

  ~FMemory();

public:

  void Allocate(VkDevice vkDevice, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryRequirements requirements,
                VkMemoryPropertyFlags memoryFlags, b8 useDeviceAddress);
  void Free();

// Getters
public:

  [[nodiscard]] VkDeviceMemory GetHandle() const { return m_DeviceMemory; }

// Members
private:

  VkDeviceMemory m_DeviceMemory{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_MEMORY_H
