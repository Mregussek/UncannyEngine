
#ifndef UNCANNYENGINE_DESCRIPTORPOOL_H
#define UNCANNYENGINE_DESCRIPTORPOOL_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FDescriptorSetLayout;


/// @brief FDescriptorPool is a wrapper for VkDescriptorPool. Also it wraps VkDescriptorSet.
/// I decided to use only one VkDescriptorSet for future applications, because it is easier to use in that way.
class FDescriptorPool
{
public:

  FDescriptorPool() = default;

  FDescriptorPool(const FDescriptorPool& other) = delete;
  FDescriptorPool(FDescriptorPool&& other) = delete;

  FDescriptorPool& operator=(const FDescriptorPool& other) = delete;
  FDescriptorPool& operator=(FDescriptorPool&& other) = delete;

  ~FDescriptorPool();

public:

  void Create(VkDevice vkDevice, const FDescriptorSetLayout* pSetLayout, u32 maxSetsCount);

  void Destroy();

  /// @brief Allocate descriptor set from pool. Remember to firstly call Create() to create pool!
  void AllocateDescriptorSet();

// Writers to descriptor
public:

  void WriteTopLevelAsToDescriptorSet(VkAccelerationStructureKHR topLevelAS, u32 dstBinding) const;

  void WriteStorageImageToDescriptorSet(VkImageView storageView, u32 dstBinding) const;

  void WriteSamplerToDescriptorSet(VkSampler sampler, VkImageView view, VkImageLayout layout, u32 dstBinding);

  void WriteBufferToDescriptorSet(VkBuffer buffer, VkDeviceSize range, u32 dstBinding,
                                  VkDescriptorType descriptorBufferType) const;

// Getters
public:

  [[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

// Members
private:

  const FDescriptorSetLayout* m_pSetLayout{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkDescriptorPool m_DescriptorPool{ VK_NULL_HANDLE };
  VkDescriptorSet m_DescriptorSet{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORPOOL_H
