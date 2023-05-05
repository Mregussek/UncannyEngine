
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
public:

  ~FDescriptorPool();

  void Create(VkDevice vkDevice, const FDescriptorSetLayout* pSetLayout, u32 maxSetsCount);

  void Destroy();

  void AllocateDescriptorSet();

  void WriteTopLevelAsToDescriptorSet(VkAccelerationStructureKHR topLevelAS, u32 dstBinding) const;

  void WriteStorageImageToDescriptorSet(VkImageView storageView, u32 dstBinding) const;

  void WriteBufferToDescriptorSet(VkBuffer buffer, VkDeviceSize range, u32 dstBinding,
                                  VkDescriptorType descriptorBufferType) const;

  [[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

private:

  const FDescriptorSetLayout* m_pSetLayout{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkDescriptorPool m_DescriptorPool{ VK_NULL_HANDLE };
  VkDescriptorSet m_DescriptorSet{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORPOOL_H
