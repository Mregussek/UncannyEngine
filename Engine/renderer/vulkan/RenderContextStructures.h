
#ifndef UNCANNYENGINE_RENDERCONTEXTSTRUCTURES_H
#define UNCANNYENGINE_RENDERCONTEXTSTRUCTURES_H


#include <utilities/Variables.h>
#include <utilities/Includes.h>
#include <volk.h>


namespace uncanny
{


struct FImageVulkan {
  VkFormat format{ VK_FORMAT_UNDEFINED };
  VkImage handle{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  VkImageView handleView{ VK_NULL_HANDLE };
};


}


#endif //UNCANNYENGINE_RENDERCONTEXTSTRUCTURES_H
