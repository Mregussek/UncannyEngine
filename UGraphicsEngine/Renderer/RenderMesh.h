
#ifndef UNCANNYENGINE_RENDERMESH_H
#define UNCANNYENGINE_RENDERMESH_H


#include "UTools/UTypes.h"
#include "UMath/Vector3.h"
#include <vector>


namespace uncanny
{


class FMeshAsset;


struct FRenderVertex
{
  math::Vector3f position{};
  math::Vector3f normal{};
  math::Vector3f color{};
};


struct FRenderMaterial
{
  math::Vector3f diffuse{};
};


struct FRenderMeshData
{
  std::vector<FRenderVertex> vertices{};
  std::vector<u32> indices{};
  math::Matrix4x4f transform{};
  u32 materialIndex{};
};


struct FRenderData
{
  std::vector<FRenderMeshData> meshes{};
  std::vector<FRenderMaterial> materials{};
};


class FRenderMeshFactory
{
public:

  static FRenderMeshData CreateTriangle();

  static FRenderData ConvertAssetToOneRenderData(const FMeshAsset* pMeshAsset);

};


}


#endif //UNCANNYENGINE_RENDERMESH_H
