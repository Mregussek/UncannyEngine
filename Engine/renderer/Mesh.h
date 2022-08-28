
#ifndef UNCANNYENGINE_MESH_H
#define UNCANNYENGINE_MESH_H


#include <math/vec3.h>


namespace uncanny
{


struct FVertex {
  vec3 position;
  vec3 color;
};


enum class EMeshType {
  NONE, TRIANGLE
};


struct FMesh {
  std::vector<FVertex> vertices{};
  EMeshType type{ EMeshType::NONE };
};


struct FMeshTriangle : public FMesh {
  FMeshTriangle() {
    vertices = {
        FVertex{ { 0.0f, -0.5f, 0.f }, { 1.0f, 0.0f, 0.0f } },
        FVertex{ { 0.5f, 0.5f, 0.f }, { 1.0f, 1.0f, 0.0f } },
        FVertex{ { -0.5f, 0.5f, 0.f }, { 1.0f, 0.0f, 1.0f } }
    };
    type = EMeshType::TRIANGLE;
  }
};


}


#endif //UNCANNYENGINE_MESH_H
