
#ifndef UNCANNYENGINE_BUFFERVULKAN_H
#define UNCANNYENGINE_BUFFERVULKAN_H


#include <utilities/Variables.h>
#include <volk.h>
#include <math/mat4.h>


namespace uncanny
{


struct FBufferVulkan {
  VkBuffer handle{ VK_NULL_HANDLE };
  VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
  VkDescriptorBufferInfo descriptorInfo{};
  u32 elemCount{ 0 };
};


struct FBufferUniformDataCameraVulkan {
  mat4 matrixModelViewProjection{};
};


}


#endif //UNCANNYENGINE_BUFFERVULKAN_H
