
#include "RenderMesh.h"
#include "UTools/Assets/MeshAsset.h"
#include <algorithm>


namespace uncanny
{


FRenderMeshData FRenderMeshFactory::CreateTriangle()
{
  return {
    .vertices = {
        { .position = { .x = 1.f, .y = 1.f, .z = 0.f },
          .normal = { 1.0f, 0.0f, 0.0f },
          .color = { 0.0f, 0.0f, 1.0f } },
        { .position = { .x = -1.f, .y = 1.f, .z = 0.f },
          .normal = { 0.0f, 1.0f, 0.0f },
          .color = { 1.0f, 0.0f, 0.0f } },
        { .position = { .x = 0.f, .y = -1.f, .z = 0.f },
          .normal = { 0.0f, 0.0f, 1.0f },
          .color = { 0.0f, 1.0f, 0.0f } },
    },
    .indices = { 0, 1, 2 }
  };
}


FRenderData FRenderMeshFactory::ConvertAssetToOneRenderData(const FMeshAsset* pMeshAsset)
{
  FRenderData rtn{};
  FRenderMeshData& meshData = rtn.meshes.emplace_back();

  u32 verticesReserveCount = 0;
  u32 indicesReserveCount = 0;
  for (const FMeshAssetData& data : pMeshAsset->GetMeshes())
  {
    verticesReserveCount += data.vertices.size();
    indicesReserveCount += data.indices.size();
  }
  meshData.vertices.reserve(verticesReserveCount);
  meshData.indices.reserve(indicesReserveCount);

  for (const FMeshAssetData& data : pMeshAsset->GetMeshes())
  {
    for (const FVertex& vertex : data.vertices)
    {
      meshData.vertices.push_back({ .position = vertex.position, .normal = vertex.normal, .color = vertex.color });
    }

    auto it = std::max_element(std::begin(meshData.indices), std::end(meshData.indices));
    u32 maxElem = it != meshData.indices.end() ? *it + 1 : 0;
    for (u32 ind : data.indices)
    {
      meshData.indices.push_back(maxElem + ind);
    }
  }
  return rtn;
}


}
