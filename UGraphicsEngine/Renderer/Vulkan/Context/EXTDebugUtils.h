
#ifndef UNCANNYENGINE_EXTDEBUGUTILS_H
#define UNCANNYENGINE_EXTDEBUGUTILS_H


#include <volk.h>


namespace uncanny::vulkan {


class FEXTDebugUtils {
public:

  void Create(VkInstance vkInstance);
  void Destroy(VkInstance vkInstance);

private:

  VkDebugUtilsMessengerEXT m_DebugUtils{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_EXTDEBUGUTILS_H
