
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

  void Create(std::span<VkDescriptorSetLayout> setLayouts);

  void Destroy();

  [[nodiscard]] VkPipelineLayout GetHandle() const { return m_PipelineLayout; }

private:

  explicit FPipelineLayout(VkDevice vkDevice);


  VkPipelineLayout m_PipelineLayout{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_PIPELINELAYOUT_H
