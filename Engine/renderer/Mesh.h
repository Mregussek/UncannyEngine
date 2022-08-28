
#ifndef UNCANNYENGINE_MESH_H
#define UNCANNYENGINE_MESH_H


#include <math/vec3.h>


namespace uncanny
{


struct FVertex {
  vec3 position;
  vec3 color;
};


struct FMeshTriangle {
  const std::array<FVertex, 3> vertices{
    FVertex{ { 0.0f, -0.5f, 0.f }, { 1.0f, 0.0f, 0.0f } },
    FVertex{ { 0.5f, 0.5f, 0.f }, { 1.0f, 1.0f, 0.0f } },
    FVertex{ { -0.5f, 0.5f, 0.f }, { 1.0f, 0.0f, 1.0f } }
  };
};


}


#endif //UNCANNYENGINE_MESH_H
