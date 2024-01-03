
#ifndef UNCANNYENGINE_PIPELINELAYOUT_H
#define UNCANNYENGINE_PIPELINELAYOUT_H


#include <volk.h>
#include <span>


namespace uncanny::vulkan
{


class FPipelineLayout
{
public:

  FPipelineLayout() = default;

  FPipelineLayout(const FPipelineLayout& other) = delete;
  FPipelineLayout(FPipelineLayout&& other) = delete;

  FPipelineLayout& operator=(const FPipelineLayout& other) = delete;
  FPipelineLayout& operator=(FPipelineLayout&& other) = delete;

  ~FPipelineLayout();

public:

  // Create methods usable for set layouts...
  void Create(VkDevice vkDevice, VkDescriptorSetLayout setLayout);
  void Create(VkDevice vkDevice, std::span<VkDescriptorSetLayout> setLayouts);

  // Create methods usable for push constants...
  void Create(VkDevice vkDevice, VkPushConstantRange pushConstantRange);
  void Create(VkDevice vkDevice, std::span<VkPushConstantRange> pushConstantRanges);

  // Create methods for both...
  void Create(VkDevice vkDevice, VkDescriptorSetLayout setLayout, VkPushConstantRange pushConstantRange);
  void Create(VkDevice vkDevice, std::span<VkDescriptorSetLayout> setLayouts,
              std::span<VkPushConstantRange> pushConstantRanges);

  void Destroy();

// Getters
public:

  [[nodiscard]] VkPipelineLayout GetHandle() const { return m_PipelineLayout; }

private:

  VkPipelineLayout m_PipelineLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PIPELINELAYOUT_H
