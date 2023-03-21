
#ifndef UNCANNYENGINE_RENDERCOMMANDS_H
#define UNCANNYENGINE_RENDERCOMMANDS_H


#include <vector>
#include <span>
#include "Commands/CommandBuffer.h"
#include "Resources/Image.h"


namespace uncanny::vulkan
{


class FRenderCommands
{
public:

  static void RecordClearColorImage(std::vector<FCommandBuffer>& commandBuffers, std::span<const VkImage> images,
                                    VkClearColorValue clearColorValue, VkImageLayout finalLayout);

  static void RecordCopyImage(std::vector<FCommandBuffer>& commandBuffers, std::span<const VkImage> srcImages,
                              std::span<const VkImage> dstImages, VkExtent2D extent, VkImageLayout finalLayout);

};


}


#endif //UNCANNYENGINE_RENDERCOMMANDS_H
