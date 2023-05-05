
#ifndef UNCANNYENGINE_PIPELINELAYOUT_H
#define UNCANNYENGINE_PIPELINELAYOUT_H


#include <volk.h>
#include <span>


namespace uncanny::vulkan
{


class FPipelineLayout
{

  friend class FRenderDeviceFactory;

public:

  FPipelineLayout() = default;
  explicit FPipelineLayout(VkDevice vkDevice);

  void Create(VkDescriptorSetLayout setLayout);
  void Create(std::span<VkDescriptorSetLayout> setLayouts);

  void Destroy();

  [[nodiscard]] VkPipelineLayout GetHandle() const { return m_PipelineLayout; }

private:

  VkPipelineLayout m_PipelineLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PIPELINELAYOUT_H
