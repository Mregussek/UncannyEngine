
#ifndef UNCANNYENGINE_GRAPHICSPIPELINE_H
#define UNCANNYENGINE_GRAPHICSPIPELINE_H


#include <volk.h>
#include "UTools/Filesystem/Path.h"
#include <span>


namespace uncanny::vulkan
{


struct FGraphicsPipelineSpecification
{
  std::span<VkVertexInputBindingDescription> vertexInputBindingDescriptions{};
  std::span<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions{};
  FPath vertexShader{ UEMPTY_PATH };
  FPath fragmentShader{ UEMPTY_PATH };
  VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
  VkRenderPass renderPass{ VK_NULL_HANDLE };
  VkDevice vkDevice{ VK_NULL_HANDLE };
  u32 targetVulkanVersion{ UUNUSED };
};


class FGraphicsPipeline
{
public:

  FGraphicsPipeline() = default;

  FGraphicsPipeline(const FGraphicsPipeline& other) = delete;
  FGraphicsPipeline(FGraphicsPipeline&& other) = delete;

  FGraphicsPipeline& operator=(const FGraphicsPipeline& other) = delete;
  FGraphicsPipeline& operator=(FGraphicsPipeline&& other) = delete;

  ~FGraphicsPipeline();

public:

  void Create(const FGraphicsPipelineSpecification& specification);
  void Destroy();

// Getters
public:

  [[nodiscard]] VkPipeline GetHandle() const { return m_Pipeline; }

private:

  void CreateCache();
  void CreatePipeline(const FGraphicsPipelineSpecification& specification);

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkPipeline m_Pipeline{ VK_NULL_HANDLE };
  VkPipelineCache m_PipelineCache{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINE_H
