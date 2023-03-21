
#ifndef UNCANNYENGINE_VOLKHANDLER_H
#define UNCANNYENGINE_VOLKHANDLER_H


#include <volk.h>


namespace uncanny::vulkan
{


/// @brief FVolkHandler is a helper class for volk library. I have decided to create this class
/// for simple volkInitialize as it looks much better during call stack.
class FVolkHandler
{
public:

  void Initialize() const;
  void LoadInstance(VkInstance vkInstance) const;

};


}


#endif //UNCANNYENGINE_VOLKHANDLER_H
