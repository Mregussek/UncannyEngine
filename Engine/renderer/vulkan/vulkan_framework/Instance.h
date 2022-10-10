
#ifndef UNCANNYENGINE_INSTANCE_H
#define UNCANNYENGINE_INSTANCE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny {
class FWindow;
}

namespace uncanny::vkf {


struct FInstanceInitDependenciesVulkan {
  u32 expectedVulkanApiVersion{ UVERSION_UNDEFINED };
  ::uncanny::FWindow* pWindow{ nullptr };
  const char* appName{ "" };
  u32 engineVersion{ UVERSION_UNDEFINED };
  u32 appVersion{ UVERSION_UNDEFINED };
};


class FInstanceVulkan {
public:

  b32 init(const FInstanceInitDependenciesVulkan& deps);
  void terminate();

private:

  VkInstance mVkInstance{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_INSTANCE_H
