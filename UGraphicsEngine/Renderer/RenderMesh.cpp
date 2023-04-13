
#include "RenderMesh.h"
#include "UTools/Assets/MeshAsset.h"


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
  for (const FSubmeshAsset& submesh : pMeshAsset->GetSubmeshes())
  {
    verticesReserveCount += submesh.positions.size();
    indicesReserveCount += submesh.indices.size();
  }
  rtn.vertices.reserve(verticesReserveCount);
  rtn.indices.reserve(indicesReserveCount);

  for (const FSubmeshAsset& submesh : pMeshAsset->GetSubmeshes())
  {
    for (math::Vector3f pos : submesh.positions)
    {
      rtn.vertices.push_back({ .position = pos });
    }

    u32 indicesSize = rtn.indices.size();
    for (u32 ind : submesh.indices)
    {
      rtn.indices.push_back(indicesSize + ind);
    }
  }
  return rtn;
}


}
