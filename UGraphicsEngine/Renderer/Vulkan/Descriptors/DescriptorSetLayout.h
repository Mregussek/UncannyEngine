
#ifndef UNCANNYENGINE_DESCRIPTORSETLAYOUT_H
#define UNCANNYENGINE_DESCRIPTORSETLAYOUT_H


#include <volk.h>
#include "UTools/UTypes.h"
#include <vector>


namespace uncanny::vulkan
{


class FDescriptorSetLayout
{
public:

  ~FDescriptorSetLayout();

  void AddBinding(u32 binding, VkDescriptorType type, u32 count, VkShaderStageFlags stageFlags,
                  const VkSampler* pImmutableSamplers);
  void AddBinding(VkDescriptorSetLayoutBinding layoutBinding);

  void Create(VkDevice vkDevice);

  void Destroy();

  [[nodiscard]] VkDescriptorSetLayout GetHandle() const { return m_DescriptorSetLayout; }

  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const { return m_Bindings; }

private:

  std::vector<VkDescriptorSetLayoutBinding> m_Bindings{};
  VkDescriptorSetLayout m_DescriptorSetLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORSETLAYOUT_H
