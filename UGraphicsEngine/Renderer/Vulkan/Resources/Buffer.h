
#ifndef UNCANNYENGINE_BUFFER_H
#define UNCANNYENGINE_BUFFER_H


#include "Memory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Queue.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


/// @details If it will be used as device local don't forget about TRANSFER_DST buffer usage flag
class FBuffer
{
public:

  FBuffer() = default;
  FBuffer(VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);
  ~FBuffer();

  void Allocate(VkDeviceSize memorySize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags);
  void Free();

  void Fill(const void* pData, u32 elementSizeof, u32 elementsCount);
  void FillStaged(const void* pData, u32 elementSizeof, u32 elementsCount, const FCommandPool& transferCommandPool,
                  const FQueue& transferQueue);

  [[nodiscard]] VkBuffer GetHandle() const { return m_Buffer; }
  [[nodiscard]] u64 GetDeviceAddress() const { return m_DeviceAddress; }
  [[nodiscard]] u32 GetFilledStride() const { return m_Stride; }
  [[nodiscard]] u32 GetFilledElementsCount() const { return m_ElementsCount; }
  [[nodiscard]] u32 GetFilledElementsSizeInBytes() const { return m_ElementsSizeInBytes; }

private:

  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkBuffer m_Buffer{ VK_NULL_HANDLE };
  VkDeviceSize m_AllocatedMemorySize{ 0 };
  u64 m_DeviceAddress{ UUNUSED };
  u32 m_Stride{ UUNUSED };
  u32 m_ElementsCount{ UUNUSED };
  u64 m_ElementsSizeInBytes{ UUNUSED };
  VkMemoryPropertyFlags m_MemoryPropertyFlags{ 0 };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_BUFFER_H
