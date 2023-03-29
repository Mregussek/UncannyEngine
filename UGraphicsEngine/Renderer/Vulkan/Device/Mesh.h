
#ifndef UNCANNYENGINE_MESH_H
#define UNCANNYENGINE_MESH_H


#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


struct FPosition
{
  f32 x{ 0.f };
  f32 y{ 0.f };
  f32 z{ 0.f };
};


struct FVertex
{
  FPosition position{};
};


}


#endif //UNCANNYENGINE_MESH_H
