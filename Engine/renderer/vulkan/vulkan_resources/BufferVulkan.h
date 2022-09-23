
#ifndef UNCANNYENGINE_BUFFERVULKAN_H
#define UNCANNYENGINE_BUFFERVULKAN_H


#include <utilities/Variables.h>
#include <volk.h>
#include <math/mat4.h>


namespace uncanny
{


enum class EBufferType {
  NONE, HOST_VISIBLE, DEVICE_WITH_STAGING
};


struct FBufferDataVulkan {
  VkBuffer handle{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  VkDeviceSize size{ 0 };
  u32 elemCount{ 0 };
  EBufferType type{ EBufferType::NONE };
  const char* logInfo{ "" };
};


struct FBufferCreateStagingDependenciesVulkan {
  VkQueue transferQueue{ VK_NULL_HANDLE };
  VkCommandPool transferCommandPool{ VK_NULL_HANDLE };
};


struct FBufferCreateDependenciesVulkan {
  void* pNext{ nullptr };
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  VkDevice device{ VK_NULL_HANDLE };
  VkDeviceSize size{ 0 };
  u32 elemCount{ 0 };
  void* pData{ nullptr };
  VkBufferUsageFlags usage{ 0 };
  EBufferType type{ EBufferType::NONE };
  const char* logInfo{ "" };
};


class FBufferVulkan {
public:

  [[nodiscard]] b32 create(const FBufferCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  [[nodiscard]] const FBufferDataVulkan& getData() const { return mData; }
  [[nodiscard]] VkDescriptorBufferInfo getDescriptorBufferInfo() const;

private:

  FBufferDataVulkan mData{};
  VkBuffer mBufferStaging{ VK_NULL_HANDLE };
  VkDeviceMemory mMemoryStaging{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_BUFFERVULKAN_H
