
#ifndef UNCANNYENGINE_UTILITIES_H
#define UNCANNYENGINE_UTILITIES_H


#include <vulkan/vulkan.h>
#include <string>


namespace uncanny::vulkan
{


std::string GetStringForVkResult(VkResult result);


void AssertVkAndThrow(VkResult result);


void AssertVkAndThrow(VkResult result, std::string_view specialMsg);


}


#endif //UNCANNYENGINE_UTILITIES_H
