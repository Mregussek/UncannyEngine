
#ifndef UNCANNYENGINE_DESCRIPTORPOOL_H
#define UNCANNYENGINE_DESCRIPTORPOOL_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Image.h"


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

  void AllocateDescriptorSets(u32 count);

  void WriteTopLevelAsToDescriptorSets(VkAccelerationStructureKHR topLevelAS, u32 dstBinding) const;

  void WriteStorageImagesToDescriptorSets(const std::vector<FImage>& images, u32 dstBinding) const;

private:

  explicit FDescriptorPool(VkDevice vkDevice);


  const FDescriptorSetLayout* m_pSetLayout{ nullptr };
  VkDescriptorPool m_DescriptorPool{ VK_NULL_HANDLE };
  std::vector<VkDescriptorSet> m_DescriptorSets{};
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORPOOL_H
