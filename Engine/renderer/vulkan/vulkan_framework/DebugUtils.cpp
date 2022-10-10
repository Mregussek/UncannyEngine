
#include "DebugUtils.h"
#include <utilities/Logger.h>


namespace uncanny::vkf {


void emplaceRequiredDebugInstanceLayers(std::vector<const char*>* pRequiredLayers) {
  UTRACE("Em-placing khronos validation layer to instance vulkan...");
  pRequiredLayers->push_back("VK_LAYER_KHRONOS_validation");
}


void emplaceRequiredDebugInstanceExtensions(std::vector<const char*>* pRequiredExtensions) {
  UTRACE("Em-placing debug utils extension to instance vulkan...");
  pRequiredExtensions->push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}


}
