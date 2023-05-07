
#include "RenderMesh.h"
#include "UTools/Assets/MeshAsset.h"
#include <algorithm>


namespace uncanny
{


FRenderData FRenderMeshFactory::CreateTriangle()
{
  FRenderData rtn{};
  rtn.mesh = {
      .vertices = {
          { .position = { .x = 1.f, .y = 1.f, .z = 0.f },
              .normal = { 1.0f, 0.0f, 0.0f } },
          { .position = { .x = -1.f, .y = 1.f, .z = 0.f },
              .normal = { 0.0f, 1.0f, 0.0f } },
          { .position = { .x = 0.f, .y = -1.f, .z = 0.f },
              .normal = { 0.0f, 0.0f, 1.0f } },
      },
      .indices = { 0, 1, 2 },
      .materialIndices = { 0, 0, 0 },
      .transform = math::Identity<f32>()
  };
  rtn.materials = { FRenderMaterialData{ .diffuse = { 0.2f, 0.5f, 0.2f } } };
  return rtn;
}


FRenderData FRenderMeshFactory::ConvertAssetToOneRenderData(const FMeshAsset* pMeshAsset, math::Matrix4x4f transform)
{
  FRenderData rtnRenderData{};

  {
    FRenderMeshData& meshData = rtnRenderData.mesh;
    meshData.transform = transform;

    // Reserve needed memory...
    meshData.materialIndices.reserve(pMeshAsset->GetMeshes().size());

    u32 verticesReserveCount = 0;
    u32 indicesReserveCount = 0;
    for (const FMeshAssetData& data : pMeshAsset->GetMeshes())
    {
      verticesReserveCount += data.vertices.size();
      indicesReserveCount += data.indices.size();
    }
    meshData.vertices.reserve(verticesReserveCount);
    meshData.indices.reserve(indicesReserveCount);

    // for every mesh process vertices and indices...
    for (const FMeshAssetData& data : pMeshAsset->GetMeshes())
    {
      for (const FVertex& vertex : data.vertices)
      {
        meshData.vertices.push_back({
          .position = vertex.position,
          .normal = vertex.normal });
      }

      auto it = std::max_element(std::begin(meshData.indices), std::end(meshData.indices));
      u32 maxElem = it != meshData.indices.end() ? *it + 1 : 0;
      for (u32 ind : data.indices)
      {
        meshData.indices.push_back(maxElem + ind);
      }

      for (u32 i = 0; i < data.indices.size(); i += 3)
      {
        meshData.materialIndices.push_back(data.materialIndex);
      }
    }
  }
  {
    rtnRenderData.materials.reserve(pMeshAsset->GetMaterials().size());

    for (const FMaterialData& data : pMeshAsset->GetMaterials())
    {
      rtnRenderData.materials.emplace_back(FRenderMaterialData{
        .ambient = data.ambient,
        .diffuse = data.diffuse,
        .specular = data.specular,
        .emissive = data.emissive,
        .reflective = data.reflective,
        .shininess = data.shininess,
        .opacity = data.opacity,
        .reflectivity = data.reflectivity,
        .indexOfRefraction = data.indexOfRefraction,
        .illuminationModel = data.illuminationModel
      });
    }
  }

  return rtnRenderData;
}


}
