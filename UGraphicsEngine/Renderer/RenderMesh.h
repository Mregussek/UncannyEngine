
#ifndef UNCANNYENGINE_RENDERMESH_H
#define UNCANNYENGINE_RENDERMESH_H


#include "UTools/UTypes.h"
#include "UMath/Matrix4x4.h"
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


struct FRenderMaterialData
{
  math::Vector3f ambient{};
  math::Vector3f diffuse{};
  math::Vector3f specular{};
  math::Vector3f emission{};
  math::Vector3f reflective{};
};


struct FRenderMeshData
{
  std::vector<FRenderVertex> vertices{};
  std::vector<u32> indices{};
  std::vector<u32> materialIndices{};
  math::Matrix4x4f transform{};
};


struct FRenderData
{
  std::vector<FRenderMeshData> meshes{};
  std::vector<FRenderMaterialData> materials{};
};


class FRenderMeshFactory
{
public:

  static FRenderMeshData CreateTriangle();

  static FRenderData ConvertAssetToOneRenderData(const FMeshAsset* pMeshAsset);

};


}


#endif //UNCANNYENGINE_RENDERMESH_H
