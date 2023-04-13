
#ifndef UNCANNYENGINE_DESCRIPTORPOOL_H
#define UNCANNYENGINE_DESCRIPTORPOOL_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FDescriptorSetLayout;


class FDescriptorPool
{

  friend class FRenderDeviceFactory;

public:

  FDescriptorPool() = default;

  void Create(const FDescriptorSetLayout* pSetLayout, u32 maxSetsCount);

  void Destroy();

  void AllocateDescriptorSet();

  void WriteTopLevelAsToDescriptorSet(VkAccelerationStructureKHR topLevelAS, u32 dstBinding) const;

  void WriteStorageImageToDescriptorSet(VkImageView storageView, u32 dstBinding) const;

  void WriteUniformBufferToDescriptorSet(VkBuffer buffer, VkDeviceSize range, u32 dstBinding) const;

  [[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

private:

  explicit FDescriptorPool(VkDevice vkDevice);


  const FDescriptorSetLayout* m_pSetLayout{ nullptr };
  VkDescriptorPool m_DescriptorPool{ VK_NULL_HANDLE };
  VkDescriptorSet m_DescriptorSet{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORPOOL_H