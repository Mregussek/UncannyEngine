
#ifndef UNCANNYENGINE_QUEUE_H
#define UNCANNYENGINE_QUEUE_H


#include <volk.h>


namespace uncanny::vulkan {


class FQueue {
public:

  void Initialize(VkQueue queue);

private:

  VkQueue m_Queue{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_QUEUE_H
