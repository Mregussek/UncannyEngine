
#ifndef UNCANNYENGINE_DESCRIPTORSETLAYOUT_H
#define UNCANNYENGINE_DESCRIPTORSETLAYOUT_H


#include <volk.h>
#include "UTools/UTypes.h"
#include <vector>


namespace uncanny::vulkan
{


class FDescriptorSetLayout
{

  friend class FRenderDeviceFactory;

public:

  FDescriptorSetLayout() = default;

  void AddBinding(u32 binding, VkDescriptorType type, u32 count, VkShaderStageFlags stageFlags,
                  const VkSampler* pImmutableSamplers);

  void Create();

  void Destroy();

  [[nodiscard]] VkDescriptorSetLayout GetHandle() const { return m_DescriptorSetLayout; }

  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const { return m_Bindings; }

private:

  explicit FDescriptorSetLayout(VkDevice vkDevice);


  std::vector<VkDescriptorSetLayoutBinding> m_Bindings{};
  VkDescriptorSetLayout m_DescriptorSetLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORSETLAYOUT_H
