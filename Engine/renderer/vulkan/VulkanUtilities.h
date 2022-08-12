
#ifndef UNCANNYENGINE_VULKANUTILITIES_H
#define UNCANNYENGINE_VULKANUTILITIES_H


#include <utilities/Variables.h>
#include <utilities/Includes.h>
#include <volk.h>


#define U_VK_DEBUG 1  // 1 - enable debug vulkan prints in UncannyEngine, 0 - disable


namespace uncanny
{


template<typename T> struct is_vk_extension_properties : std::false_type { };
template<> struct is_vk_extension_properties<VkExtensionProperties> : std::true_type { };

template<typename T> struct is_vk_layer_properties : std::false_type { };
template<> struct is_vk_layer_properties<VkLayerProperties> : std::true_type { };


struct FDriverVersionInfo {
  u32 major{ 0 };
  u32 minor{ 0 };
  u32 patch{ 0 };
};


u32 retrieveVulkanApiVersion();


FDriverVersionInfo decodeDriverVersionVulkan(u32 version, u32 vendorID);


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


#define U_VK_ASSERT(call) __VK_CHECK_TEMPLATE(call, __debugbreak())


#endif //UNCANNYENGINE_VULKANUTILITIES_H
