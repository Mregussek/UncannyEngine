
#ifndef UNCANNYENGINE_RENDERCONTEXT_H
#define UNCANNYENGINE_RENDERCONTEXT_H


#include <utilities/Includes.h>


namespace uncanny
{


class FWindow;


enum class EPhysicalDeviceType {
  NONE, INTEGRATED, DISCRETE
};


struct FQueueFamilyDependencies {
  // @brief how many queues of such type are needed
  u32 queuesCountNeeded{ 0 };
  // @brief support graphics operations
  u32 graphics{ UFALSE };
  // @brief support compute operations
  u32 compute{ UFALSE };
  // @brief support transfer operations
  u32 transfer{ UFALSE };
  // @brief support sparse memory management operations
  u32 sparseBinding{ UFALSE };
};


struct FPhysicalDeviceDependencies {
  // @brief what type of device do you prefer -> integrated / discrete
  EPhysicalDeviceType deviceType{ EPhysicalDeviceType::NONE };
  // @brief if above is not available, what fallback device is preferred
  EPhysicalDeviceType deviceTypeFallback{ EPhysicalDeviceType::NONE };
  // @brief define, whether presentation on the screen should be supported
  u32 presentationSupport{ UFALSE };
  // @brief how many queue family indexes types are needed (for graphics, for transfer etc.)
  u32 queueFamilyIndexesCount{ 0 };
  // @brief dependencies for queue families. Count should be equal to queueFamilyIndexesNeeded
  FQueueFamilyDependencies* pQueueFamilyDependencies{ nullptr };
};


struct FRenderContextSpecification {
  FWindow* pWindow{ nullptr };
  FPhysicalDeviceDependencies physicalDeviceDependencies{};
  const char* pAppName{ nullptr };
  u32 appVersion{ 0 };
  u32 engineVersion{ 0 };
};


class FRenderContext {
public:

  virtual b32 init(FRenderContextSpecification renderContextSpecs);
  virtual void terminate();

};


}


#endif //UNCANNYENGINE_RENDERCONTEXT_H
