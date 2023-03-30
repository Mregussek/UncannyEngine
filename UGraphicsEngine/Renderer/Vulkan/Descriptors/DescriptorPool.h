
#ifndef UNCANNYENGINE_DESCRIPTORPOOL_H
#define UNCANNYENGINE_DESCRIPTORPOOL_H


#include "DescriptorSetLayout.h"


namespace uncanny::vulkan
{


class FDescriptorPool
{

  friend class FRenderDeviceFactory;

public:

  FDescriptorPool() = default;

  void Create(const FDescriptorSetLayout& setLayout, u32 maxSetsCount);

  void Destroy();

private:

  explicit FDescriptorPool(VkDevice vkDevice);


  VkDescriptorPool m_DescriptorPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORPOOL_H
