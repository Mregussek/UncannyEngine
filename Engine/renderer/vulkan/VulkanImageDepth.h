
#ifndef UNCANNYENGINE_VULKANIMAGEDEPTH_H
#define UNCANNYENGINE_VULKANIMAGEDEPTH_H


#include "RenderContextVulkan.h"


namespace uncanny
{


b32 detectSupportedDepthFormat(VkPhysicalDevice physicalDevice,
                               const std::vector<VkFormat>& depthFormatCandidates,
                               VkFormat* pOutDepthFormat);


}


#endif //UNCANNYENGINE_VULKANIMAGEDEPTH_H
