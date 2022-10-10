
#ifndef UNCANNYENGINE_DEBUGUTILS_H
#define UNCANNYENGINE_DEBUGUTILS_H


#include <vector>
#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf {


void emplaceRequiredDebugInstanceLayers(std::vector<const char*>* pRequiredLayers);


void emplaceRequiredDebugInstanceExtensions(std::vector<const char*>* pRequiredExtensions);


struct FDebugUtilsInitDependenciesVulkan {
  VkInstance instance{ VK_NULL_HANDLE };
};


class FDebugUtilsVulkan {
public:

  b32 init(const FDebugUtilsInitDependenciesVulkan& deps);
  void terminate(VkInstance instance);

private:

  VkDebugUtilsMessengerEXT mVkDebugUtilsMsg{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_DEBUGUTILS_H
