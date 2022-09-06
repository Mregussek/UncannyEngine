
#ifndef UNCANNYENGINE_RENDERERSTRUCTURESVULKAN_H
#define UNCANNYENGINE_RENDERERSTRUCTURESVULKAN_H


#include <utilities/Variables.h>
#include <volk.h>
#include <math/mat4.h>


namespace uncanny
{


enum class EImageType {
  NONE, PRESENTABLE, DEPTH, RENDER_TARGET
};


struct FImageVulkan {
  VkImage handle{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  VkImageView handleView{ VK_NULL_HANDLE };
  VkFramebuffer handleFramebuffer{ VK_NULL_HANDLE };
  VkExtent3D extent{ 0, 0, 0 };
  VkFormat format{ VK_FORMAT_UNDEFINED };
  VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
  EImageType type{ EImageType::NONE };
};


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
