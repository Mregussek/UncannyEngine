
#ifndef UNCANNYENGINE_INSTANCE_H
#define UNCANNYENGINE_INSTANCE_H


#include <volk.h>


namespace uncanny::vulkan {


class FInstanceProperties;


class FInstance {
public:

  ~FInstance();

  void Create(const FInstanceProperties& properties);
  void Destroy();

private:

  VkInstance m_Instance;

};


}

#endif //UNCANNYENGINE_INSTANCE_H
