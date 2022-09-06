
#ifndef UNCANNYENGINE_ALLOCATIONS_H
#define UNCANNYENGINE_ALLOCATIONS_H


#include <utilities/Variables.h>
#include <volk.h>


namespace uncanny
{


enum class EMemoryCopyType {
  NONE, USE_HOST_MEMCPY, USE_COMMAND_BUFFER
};


struct FMemoryAllocationDependenciesVulkan {
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  VkDevice device{ VK_NULL_HANDLE };
  VkBuffer buffer{ VK_NULL_HANDLE };
  VkMemoryPropertyFlags propertyFlags{ 0 };
  const char* logInfo{ "" };
};


struct FMemoryCopyBuffersUsingStagingBufferDependenciesVulkan {
  VkBuffer srcBuffer{ VK_NULL_HANDLE };
  VkBuffer dstBuffer{ VK_NULL_HANDLE };
  VkQueue transferQueue{ VK_NULL_HANDLE };
  VkCommandPool transferCommandPool{ VK_NULL_HANDLE };
  const char* srcLogInfo{ "" };
  const char* dstLogInfo{ "" };
};


struct FMemoryCopyDependenciesVulkan {
  void* pNext{ nullptr };
  VkDevice device{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  VkDeviceSize size{ 0 };
  void* pData{ nullptr };
  EMemoryCopyType copyType{ EMemoryCopyType::NONE };
  const char* logInfo{ "" };
};


class FMemoryVulkan {
public:

  static b32 allocate(const FMemoryAllocationDependenciesVulkan& deps,
                      VkDeviceMemory* pOutDeviceMemoryHandle);

  static b32 copy(const FMemoryCopyDependenciesVulkan& deps);

private:

  static b32 findMemoryIndex(VkPhysicalDevice physicalDevice, u32 typeFilter,
                             VkMemoryPropertyFlags flags, u32* pOutIndex);

  static u32 sNumOfAllocations;

};


}


#endif //UNCANNYENGINE_ALLOCATIONS_H
