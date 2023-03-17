
#ifndef UNCANNYENGINE_RENDERCOMMANDS_H
#define UNCANNYENGINE_RENDERCOMMANDS_H


#include <vector>
#include "Commands/CommandBuffer.h"


namespace uncanny::vulkan
{


class FRenderCommands
{
public:

  static void RecordClearColorImage(std::vector<FCommandBuffer>& commandBuffers, const std::vector<VkImage>& images,
                                    VkClearColorValue clearColorValue);

};


}


#endif //UNCANNYENGINE_RENDERCOMMANDS_H
