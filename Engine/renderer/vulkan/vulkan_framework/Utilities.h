
#ifndef UNCANNYENGINE_UTILITIES_H
#define UNCANNYENGINE_UTILITIES_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf {


void AssertVulkan(b32 boolean, VkResult result);


void AssertResultVulkan(VkResult result);


const char* retrieveInfoAboutVkResult(VkResult result);


}


#endif //UNCANNYENGINE_UTILITIES_H
