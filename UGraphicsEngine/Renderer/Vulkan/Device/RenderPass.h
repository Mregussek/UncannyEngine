
#ifndef UNCANNYENGINE_RENDERPASS_H
#define UNCANNYENGINE_RENDERPASS_H


#include <volk.h>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/// @brief FRenderPass is a wrapper for VkRenderPass.
class FRenderPass
{
public:

  FRenderPass() = default;

  FRenderPass(const FRenderPass& other) = delete;
  FRenderPass(FRenderPass&& other) = delete;

  FRenderPass& operator=(const FRenderPass& other) = delete;
  FRenderPass& operator=(FRenderPass&& other) = delete;

  ~FRenderPass();

public:

  void Create(b32 clearColorAttachmentEveryDraw, VkFormat swapchainFormat, VkFormat depthFormat,
              VkDevice vkDevice);

  void Destroy();

// Getters
public:

  [[nodiscard]] VkRenderPass GetHandle() const { return m_RenderPass; }

// Members
private:

  VkDevice m_Device{ VK_NULL_HANDLE };
  VkRenderPass m_RenderPass{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_RENDERPASS_H
