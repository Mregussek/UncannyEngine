
#ifndef UNCANNYENGINE_VOLKHANDLER_H
#define UNCANNYENGINE_VOLKHANDLER_H


#include <volk.h>


namespace uncanny::vulkan
{


/*
 * @brief FVolkHandler is a helper class for volk library. I have decided to create this class
 * for simple volkInitialize as it looks much better during Renderer.Initialize().
 */
class FVolkHandler
{
public:

  void Initialize();
  void LoadInstance(VkInstance vkInstance);

};


}


#endif //UNCANNYENGINE_VOLKHANDLER_H
