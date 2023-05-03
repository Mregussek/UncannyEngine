
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
  // Ambient color of the material. Typically scaled by the amount of ambient light
  math::Vector3f ambient{};
  // Diffuse color of the material. Typically scaled by the amount of incoming diffuse light (e.g. gouraud shading)
  math::Vector3f diffuse{};
  // Specular color of the material. Typically scaled by the amount of incoming specular light (e.g. phong shading)
  math::Vector3f specular{};
  // Emissive color of the material. Amount of light emitted by the object. In real time applications it will usually
  // not affect surrounding objects, but raytracing applications may wish to treat emissive objects as light sources.
  math::Vector3f emission{};
  // Defines the shininess of a phong-shaded material. This is actually the exponent of the phong specular equation
  f32 specularShininess{ 0.f };
  // Defines the Index Of Refraction for the material.
  f32 indexOfRefraction{ 0.f };
  // Phong, Blinn, Gouraud etc.
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

  static FRenderMeshData CreateTriangle();

  static FRenderData ConvertAssetToOneRenderData(const FMeshAsset* pMeshAsset, math::Matrix4x4f transform);

};


}


#endif //UNCANNYENGINE_RENDERMESH_H
