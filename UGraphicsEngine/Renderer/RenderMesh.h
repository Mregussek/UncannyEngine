
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
};


struct FRenderMaterialData
{
  math::Vector3f ambient{};
  math::Vector3f diffuse{};
  math::Vector3f specular{};
  math::Vector3f emissive{};
  math::Vector3f transparent{};
  f32 shininess{ 0.f };
  f32 opacity{ 0.f };
  f32 indexOfRefraction{ 0.f };
  i32 illuminationModel{ 0 };
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
  FRenderMeshData mesh{};
  std::vector<FRenderMaterialData> materials{};
};


class FRenderMeshFactory
{
public:

  static FRenderData CreateTriangle();

  static FRenderData ConvertAssetToOneRenderData(const FMeshAsset* pMeshAsset, math::Matrix4x4f transform);

};


}


#endif //UNCANNYENGINE_RENDERMESH_H
