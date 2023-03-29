
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

  void Allocate(VkDeviceSize memorySize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
  void Free();

  void Fill(void* pData, u32 elementSizeof, u32 elementsCount);
  void FillStaged(void* pData, u32 elementSizeof, u32 elementsCount, const FCommandPool& transferCommandPool,
                  const FQueue& transferQueue);

  [[nodiscard]] VkBuffer GetHandle() const { return m_Buffer; }
  [[nodiscard]] u64 GetDeviceAddress() const { return m_Address; }
  [[nodiscard]] u32 GetStride() const { return m_ElementSizeof; }
  [[nodiscard]] u32 GetElementsCount() const { return m_ElementsCount; }

private:

  FBuffer(const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, VkDevice vkDevice);


  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkBuffer m_Buffer{ VK_NULL_HANDLE };
  VkDeviceSize m_MemorySize{ 0 };
  u64 m_Address{ UUNUSED };
  u32 m_ElementSizeof{ UUNUSED };
  u32 m_ElementsCount{ UUNUSED };
  VkMemoryPropertyFlags m_MemoryFlags{ 0 };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_BUFFER_H
