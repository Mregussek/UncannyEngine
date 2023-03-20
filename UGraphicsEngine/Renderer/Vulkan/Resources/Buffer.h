
#ifndef UNCANNYENGINE_BUFFER_H
#define UNCANNYENGINE_BUFFER_H


#include "Memory.h"


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


class FBuffer
{

  friend class FRenderDeviceFactory;

public:

  FBuffer() = default;

  void Allocate(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
  void Free();

private:

  FBuffer(const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, VkDevice vkDevice);


  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkBuffer m_Buffer{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_BUFFER_H
