
#ifndef UNCANNYENGINE_EXTDEBUGUTILS_H
#define UNCANNYENGINE_EXTDEBUGUTILS_H


#include <volk.h>


namespace uncanny::vulkan {


class FInstance;


class FEXTDebugUtils {
public:

  void Create(const FInstance& instance);
  void Destroy(const FInstance& instance);

private:

  VkDebugUtilsMessengerEXT m_DebugUtils{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_EXTDEBUGUTILS_H
