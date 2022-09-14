
#ifndef UNCANNYENGINE_RENDERERSTRUCTURESVULKAN_H
#define UNCANNYENGINE_RENDERERSTRUCTURESVULKAN_H


#include <utilities/Variables.h>
#include <volk.h>
#include <math/mat4.h>


namespace uncanny
{


struct FCameraUBO {
  mat4 matrixModelViewProjection{};
};


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


#endif //UNCANNYENGINE_RENDERERSTRUCTURESVULKAN_H
