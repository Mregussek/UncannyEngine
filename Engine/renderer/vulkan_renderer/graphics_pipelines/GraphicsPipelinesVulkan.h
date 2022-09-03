
#ifndef UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
#define UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H


#include <volk.h>
#include <vector>


namespace uncanny
{


struct FGraphicsPipelineVulkan {
  VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
  VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
  std::vector<VkDescriptorSet> descriptorSetVector{};
  VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
  VkPipeline pipeline{ VK_NULL_HANDLE };
  VkViewport viewport{};
  VkRect2D scissor{};
};


}


#endif //UNCANNYENGINE_GRAPHICSPIPELINESVULKAN_H
