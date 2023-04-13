
#include "AssetRegistry.h"
#include "UTools/Logger/Log.h"


namespace uncanny
{


FMeshAsset& FAssetRegistry::RegisterMesh()
{
  u64 uniqueID = m_IDRegistry.GenerateID();
  auto [it, inserted] = m_MeshAssets.emplace(std::make_pair(uniqueID, FMeshAsset{ uniqueID }));
  if (not inserted)
  {
    UWARN("Asset with ID {} is already inserted!");
  }
  return m_MeshAssets.at(uniqueID);
}


void FAssetRegistry::Clear()
{
  m_MeshAssets.clear();
}


}
