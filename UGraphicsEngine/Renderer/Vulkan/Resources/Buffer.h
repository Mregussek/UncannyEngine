
#ifndef UNCANNYENGINE_BUFFER_H
#define UNCANNYENGINE_BUFFER_H


#include "Memory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Queue.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


/// @brief FBuffer is a wrapper for VkBuffer. It also is responsible for memory management for the buffer.
class FBuffer
{
public:

  FBuffer() = default;

  FBuffer(const FBuffer& other) = delete;
  FBuffer(FBuffer&& other) = default;

  FBuffer& operator=(const FBuffer& other) = delete;
  FBuffer& operator=(FBuffer&& other) = delete;

  ~FBuffer();

public:

  /// @brief Creates a VkBuffer with specified parameters and allocates memory for it.
  /// @param usage If it will be used as device local don't forget about TRANSFER_DST buffer usage flag
  void Allocate(VkDeviceSize memorySize, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags,
                VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);
  void Free();

  /// @brief Maps the buffer memory to the host memory.
  /// @return void pointer to the mapped memory.
  void* Map();

  /// @brief Unmaps the buffer memory from the host memory.
  /// @details Also if memoryPropertyFlag VK_MEMORY_PROPERTY_HOST_COHERENT_BIT has been set, it flushes the memory.
  void Unmap();

  /// @brief Fills the buffer with data on the host side (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT).
  /// @param elementSizeof size of one element in bytes.
  /// @param elementsCount count of elements to fill.
  void Fill(const void* pData, u32 elementSizeof, u32 elementsCount);

  /// @brief Fills the buffer with data on the device local side (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).
  /// This method may be used to fill the buffer on the host side, but it is not recommended (performance reasons).
  /// @param elementSizeof size of one element in bytes.
  /// @param elementsCount count of elements to fill.
  void FillStaged(const void* pData, u32 elementSizeof, u32 elementsCount, const FCommandPool& transferCommandPool,
                  const FQueue& transferQueue);

  /// @brief Checks whether the buffer is valid.
  [[nodiscard]] b32 IsValid() const;

// Getters
public:

  [[nodiscard]] VkBuffer GetHandle() const { return m_Buffer; }
  [[nodiscard]] VkDeviceSize GetAllocatedSize() const { return m_AllocatedMemorySize; }
  [[nodiscard]] u32 GetFilledStride() const { return m_Stride; }
  [[nodiscard]] u32 GetFilledElementsCount() const { return m_ElementsCount; }

  /// @brief If user did not pass VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT to Allocate() method,
  /// this method will return UUNUSED.
  [[nodiscard]] u64 GetDeviceAddress() const { return m_DeviceAddress; }

// Members
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

};


}


#endif //UNCANNYENGINE_BUFFER_H
