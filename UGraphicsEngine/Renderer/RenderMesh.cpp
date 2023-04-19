
#include "RenderMesh.h"
#include "UTools/Assets/MeshAsset.h"
#include <algorithm>


namespace uncanny
{


FRenderMesh FRenderMeshFactory::CreateTriangle()
{
  return {
    .vertices = {
        { .position = { .x = 1.f, .y = 1.f, .z = 0.f } },
        { .position = { .x = -1.f, .y = 1.f, .z = 0.f } },
        { .position = { .x = 0.f, .y = -1.f, .z = 0.f } },
    },
    .indices = { 0, 1, 2 }
  };
}


FRenderMesh FRenderMeshFactory::ConvertAsset(const FMeshAsset* pMeshAsset)
{
  FRenderMesh rtn{};

  u32 verticesReserveCount = 0;
  u32 indicesReserveCount = 0;
  for (const FMeshAssetData& data : pMeshAsset->GetData())
  {
    verticesReserveCount += data.vertices.size();
    indicesReserveCount += data.indices.size();
  }
  rtn.vertices.reserve(verticesReserveCount);
  rtn.indices.reserve(indicesReserveCount);

  for (const FMeshAssetData& data : pMeshAsset->GetData())
  {
    for (const FVertex& vertex : data.vertices)
    {
      rtn.vertices.push_back({ .position = vertex.position });
    }

    auto it = std::max_element(std::begin(rtn.indices), std::end(rtn.indices));
    u32 maxElem = it != rtn.indices.end() ? *it + 1 : 0;
    for (u32 ind : data.indices)
    {
      rtn.indices.push_back(maxElem + ind);
    }
  }
  return rtn;
}


}
