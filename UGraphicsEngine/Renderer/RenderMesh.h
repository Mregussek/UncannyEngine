
#ifndef UNCANNYENGINE_RENDERMESH_H
#define UNCANNYENGINE_RENDERMESH_H


#include "UTools/UTypes.h"
#include <vector>


namespace uncanny
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


struct FRenderMesh
{
  std::vector<FVertex> vertices{};
  std::vector<u32> indices{};
};


class FRenderMeshFactory
{
public:

  static FRenderMesh CreateTriangle();

};


}


#endif //UNCANNYENGINE_RENDERMESH_H
