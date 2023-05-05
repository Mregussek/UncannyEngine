
#ifndef UNCANNYENGINE_PIPELINELAYOUT_H
#define UNCANNYENGINE_PIPELINELAYOUT_H


#include <volk.h>
#include <span>


namespace uncanny::vulkan
{


class FPipelineLayout
{
public:

  ~FPipelineLayout();

  void Create(VkDevice vkDevice, VkDescriptorSetLayout setLayout);
  void Create(VkDevice vkDevice, std::span<VkDescriptorSetLayout> setLayouts);

  void Destroy();

  [[nodiscard]] VkPipelineLayout GetHandle() const { return m_PipelineLayout; }

private:

  VkPipelineLayout m_PipelineLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PIPELINELAYOUT_H
