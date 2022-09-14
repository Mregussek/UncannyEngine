
#ifndef UNCANNYENGINE_VULKANIMAGES_H
#define UNCANNYENGINE_VULKANIMAGES_H


#include <utilities/Variables.h>
#include <utilities/Includes.h>
#include <volk.h>


namespace uncanny
{


struct FImageVulkan;


b32 closeImageVulkan(FImageVulkan* pImage, VkDevice device, const char* logInfo);


b32 areFormatsFeaturesDependenciesMetForImageFormat(
    VkFormat imageFormat, VkImageTiling tiling, VkPhysicalDevice physicalDevice,
    const std::vector<VkFormatFeatureFlags>& formatFeatureVector, const char* logInfo);


b32 allocateAndBindImageMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                               FImageVulkan* pImage, const char* logInfo);


}


#endif //UNCANNYENGINE_VULKANIMAGES_H