
#ifndef UNCANNYENGINE_EXTDEBUGUTILS_H
#define UNCANNYENGINE_EXTDEBUGUTILS_H


#include <volk.h>


namespace uncanny::vulkan
{


/*
 * @brief FEXTDebugUtils is wrapper class for DebugUtilsMessenger handle in Vulkan.
 * Must be used only in RenderContext. RenderContext is responsible for lifetime.
 */
class FEXTDebugUtils
{
public:

  void Create(VkInstance vkInstance);
  void Destroy(VkInstance vkInstance);

private:

  VkDebugUtilsMessengerEXT m_DebugUtils{ VK_NULL_HANDLE };
  //VkDebugReportCallbackEXT m_DebugReport{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_EXTDEBUGUTILS_H
