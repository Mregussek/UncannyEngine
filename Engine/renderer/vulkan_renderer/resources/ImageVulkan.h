
#ifndef UNCANNYENGINE_IMAGEVULKAN_H
#define UNCANNYENGINE_IMAGEVULKAN_H


#include <volk.h>


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


}


#endif //UNCANNYENGINE_IMAGEVULKAN_H
