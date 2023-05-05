
#ifndef UNCANNYENGINE_MEMORY_H
#define UNCANNYENGINE_MEMORY_H


#include <volk.h>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @details Memory should not be exposed to the end user, FBuffer and FImage should be owners.
class FMemory
{
public:

  void Allocate(VkDevice vkDevice, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryRequirements requirements,
                VkMemoryPropertyFlags memoryFlags, b8 useDeviceAddress);
  void Free();

  [[nodiscard]] VkDeviceMemory GetHandle() const { return m_DeviceMemory; }

private:

  VkDeviceMemory m_DeviceMemory{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_MEMORY_H
