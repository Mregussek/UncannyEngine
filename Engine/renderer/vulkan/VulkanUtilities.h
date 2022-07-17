
#ifndef UNCANNYENGINE_VULKANUTILITIES_H
#define UNCANNYENGINE_VULKANUTILITIES_H


#include <utilities/Variables.h>
#include <volk.h>


#define U_VK_DEBUG 1  // 1 - enable debug vulkan prints in UncannyEngine, 0 - disable


namespace uncanny
{


u32 retrieveVulkanApiVariantVersion(u32 version);

u32 retrieveVulkanApiMajorVersion(u32 version);

u32 retrieveVulkanApiMinorVersion(u32 version);

u32 retrieveVulkanApiVersion();

const char* retrieveInfoAboutVkResult(VkResult result);


}


#define VK_CHECK(call) \
    do { \
        const VkResult result = call; \
        if (result != VK_SUCCESS) { \
          const char* msg{ ::uncanny::retrieveInfoAboutVkResult(result) }; \
          UERROR("VkResult {} : {}", result, msg); \
          __debugbreak(); \
        } \
    } while(0)


#endif //UNCANNYENGINE_VULKANUTILITIES_H
