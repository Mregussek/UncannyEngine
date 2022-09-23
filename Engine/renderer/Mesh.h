
#ifndef UNCANNYENGINE_MESH_H
#define UNCANNYENGINE_MESH_H


#include <math/vec3.h>
#include <math/mat4.h>


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
  mat4 transformLocal{};

  [[nodiscard]] FVertex* getVerticesData() { return mVertices.data(); }
  [[nodiscard]] u32 getVerticesSizeof() const { return sizeof(mVertices[0]) * getVerticesCount(); }
  [[nodiscard]] u32 getVerticesCount() const { return mVertices.size(); }

  [[nodiscard]] u32* getIndicesData() { return mIndices.data(); }
  [[nodiscard]] u32 getIndicesSizeof() const { return sizeof(mIndices[0]) * getIndicesCount(); }
  [[nodiscard]] u32 getIndicesCount() const { return mIndices.size(); }

  [[nodiscard]] EMeshType getType() const { return mType; }

protected:

  std::vector<FVertex> mVertices{};
  std::vector<u32> mIndices{};
  EMeshType mType{ EMeshType::NONE };

};


struct FMeshQuads : public FMesh {
  FMeshQuads() {
    mVertices = {
        FVertex{ { -0.5f, -0.5f, 0.f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, -0.5f, 0.f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, 0.5f, 0.f }, { 0.0f, 0.0f, 1.0f } },
        FVertex{ { -0.5f, 0.5f, 0.f }, { 0.0f, 0.0f, 1.0f } },

        FVertex{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        FVertex{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },

        FVertex{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
        FVertex{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
        FVertex{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } }
    };
    mIndices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8
    };
    mType = EMeshType::TRIANGLE;
  }
};


}


#endif //UNCANNYENGINE_MESH_H
