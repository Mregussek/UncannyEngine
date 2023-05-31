
#ifndef UNCANNYENGINE_RENDERPASS_H
#define UNCANNYENGINE_RENDERPASS_H


#include <volk.h>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FRenderPass
{
public:

  ~FRenderPass();

  void Create(b32 clearColorAttachmentEveryDraw, VkFormat swapchainFormat, VkFormat depthFormat, VkDevice vkDevice);
  void Destroy();

  [[nodiscard]] VkRenderPass GetHandle() const { return m_RenderPass; }

private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkRenderPass m_RenderPass{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RENDERPASS_H
