
#ifndef UNCANNYENGINE_UTILITIES_H
#define UNCANNYENGINE_UTILITIES_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf
{


void AssertResultVulkan(VkResult result);


struct FDriverVersionInfo {
  u32 major{ 0 };
  u32 minor{ 0 };
  u32 patch{ 0 };
};


FDriverVersionInfo decodeDriverVersionVulkan(u32 version, u32 vendorID);


const char* retrieveInfoAboutVkResult(VkResult result);


}


#endif //UNCANNYENGINE_UTILITIES_H
