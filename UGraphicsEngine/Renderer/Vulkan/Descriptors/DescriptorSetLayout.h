
#ifndef UNCANNYENGINE_DESCRIPTORSETLAYOUT_H
#define UNCANNYENGINE_DESCRIPTORSETLAYOUT_H


#include <volk.h>
#include "UTools/UTypes.h"
#include <vector>


namespace uncanny::vulkan
{


/// @brief FDescriptorSetLayout is a wrapper for VkDescriptorSetLayout.
class FDescriptorSetLayout
{
public:

  FDescriptorSetLayout() = default;

  FDescriptorSetLayout(const FDescriptorSetLayout& other) = delete;
  FDescriptorSetLayout(FDescriptorSetLayout&& other) = delete;

  FDescriptorSetLayout& operator=(const FDescriptorSetLayout& other) = delete;
  FDescriptorSetLayout& operator=(FDescriptorSetLayout&& other) = delete;

  ~FDescriptorSetLayout();

public:

  void AddBinding(u32 binding, VkDescriptorType type, u32 count, VkShaderStageFlags stageFlags,
                  const VkSampler* pImmutableSamplers);
  void AddBinding(VkDescriptorSetLayoutBinding layoutBinding);

  /// @brief AddBinding() method is used to add bindings to the layout and should be used before Create() method.
  void Create(VkDevice vkDevice);

  void Destroy();

// Getters
public:

  [[nodiscard]] VkDescriptorSetLayout GetHandle() const { return m_DescriptorSetLayout; }

  [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const { return m_Bindings; }

// Members
private:

  std::vector<VkDescriptorSetLayoutBinding> m_Bindings{};
  VkDescriptorSetLayout m_DescriptorSetLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DESCRIPTORSETLAYOUT_H
