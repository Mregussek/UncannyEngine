
#ifndef UNCANNYENGINE_BUFFER_H
#define UNCANNYENGINE_BUFFER_H


#include "Memory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Queue.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


/*
 * @brief
 * If it will be used as device local don't forget about TRANSFER_DST buffer usage flag
 */
class FBuffer
{

  friend class FRenderDeviceFactory;

public:

  FBuffer() = default;
  ~FBuffer();

  void Allocate(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
  void Free();

  void Fill(void* pData, VkDeviceSize dataSize);
  void FillStaged(void* pData, VkDeviceSize dataSize, const FCommandPool& transferCommandPool,
                  const FQueue& transferQueue);

private:

  FBuffer(const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, VkDevice vkDevice);


  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkBuffer m_Buffer{ VK_NULL_HANDLE };
  VkDeviceSize m_Size{ 0 };
  VkMemoryPropertyFlags m_MemoryFlags{ 0 };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_BUFFER_H
