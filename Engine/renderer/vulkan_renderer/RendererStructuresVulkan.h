
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


struct FBufferVulkan {
  VkBuffer handle{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  u32 elemCount{ 0 };
};


struct FCameraUBO {
  mat4 matrixModelViewProjection{};
};


}


#endif //UNCANNYENGINE_RENDERERSTRUCTURESVULKAN_H
