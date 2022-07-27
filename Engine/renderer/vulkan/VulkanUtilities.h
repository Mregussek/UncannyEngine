
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


#define __VK_CHECK_TEMPLATE(call, rtnmode) \
  do { \
    const VkResult result = call; \
    if (result != VK_SUCCESS) { \
      const char* info{ ::uncanny::retrieveInfoAboutVkResult(result) }; \
      UERROR("VkResult {} : {}", result, info); \
      rtnmode; \
    } \
  } while(0)


#define URETURN_FALSE_IF_NOT_VK_SUCCESS(call) __VK_CHECK_TEMPLATE(call, return UFALSE)

#define U_VK_ASSERT(call) __VK_CHECK_TEMPLATE(call, __debugbreak())

#endif //UNCANNYENGINE_VULKANUTILITIES_H
