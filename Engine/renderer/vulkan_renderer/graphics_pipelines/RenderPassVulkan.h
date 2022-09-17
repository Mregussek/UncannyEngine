
#ifndef UNCANNYENGINE_RENDERPASS_H
#define UNCANNYENGINE_RENDERPASS_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny
{


struct FRenderPassDataVulkan {
  VkRenderPass renderPass{ VK_NULL_HANDLE };
  VkFormat renderTargetFormat{ VK_FORMAT_UNDEFINED };
  VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
  const char* logInfo{ "" };
};


struct FRenderPassCreateDependenciesVulkan {
  VkDevice device{ VK_NULL_HANDLE };
  VkFormat renderTargetFormat{ VK_FORMAT_UNDEFINED };
  VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
  const char* logInfo{ "" };
};


class FRenderPassVulkan {
public:

  b32 create(const FRenderPassCreateDependenciesVulkan& deps);
  b32 close(VkDevice device);

  [[nodiscard]] const FRenderPassDataVulkan& getData() const { return mData; }

private:

  FRenderPassDataVulkan mData{};

};


}


#endif //UNCANNYENGINE_RENDERPASS_H
