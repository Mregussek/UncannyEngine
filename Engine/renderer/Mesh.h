
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
  std::vector<u32> indices{};
  EMeshType type{ EMeshType::NONE };
};


struct FMeshTriangle : public FMesh {
  FMeshTriangle() {
    vertices = {
        FVertex{ { -0.5f, -0.5f, 0.f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, -0.5f, 0.f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, 0.5f, 0.f }, { 0.0f, 0.0f, 1.0f } },
        FVertex{ { -0.5f, 0.5f, 0.f }, { 0.0f, 0.0f, 1.0f } }
    };
    indices = {
        0, 1, 2,
        2, 3, 0
    };
    type = EMeshType::TRIANGLE;
  }
};


}


#endif //UNCANNYENGINE_MESH_H
