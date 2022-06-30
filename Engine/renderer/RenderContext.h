
#ifndef UNCANNYENGINE_RENDERCONTEXT_H
#define UNCANNYENGINE_RENDERCONTEXT_H


#include <volk.h>


namespace uncanny
{


class FRenderContext {
  VkInstance m_instance{ VK_NULL_HANDLE };
  VkDebugReportCallbackEXT m_callback{ VK_NULL_HANDLE };
};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
