
#include "MeshAsset.h"


namespace uncanny
{


FMeshAsset::FMeshAsset(u64 id)
  : m_ID(id)
{
}


FSubmeshAsset& FMeshAsset::AppendSubmesh()
{
  return m_Submeshes.emplace_back();
}


}
