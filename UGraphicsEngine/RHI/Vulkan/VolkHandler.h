
#ifndef UNCANNYENGINE_VOLKHANDLER_H
#define UNCANNYENGINE_VOLKHANDLER_H


namespace uncanny::vulkan {


/*
 * @brief FVolkHandler is a helper class for volk library. I have decided to create this class
 * for simple volkInitialize as it looks much better during RHI.Initialize().
 */
class FVolkHandler {
public:

  void Create();
  void Destroy();

};


}


#endif //UNCANNYENGINE_VOLKHANDLER_H
