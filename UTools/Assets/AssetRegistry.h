
#ifndef UNCANNYENGINE_ASSETREGISTRY_H
#define UNCANNYENGINE_ASSETREGISTRY_H


#include "MeshAsset.h"
#include "IdentifierRegistry.h"
#include <variant>
#include <unordered_map>


namespace uncanny
{


class FAssetRegistry
{
public:

  FMeshAsset& RegisterMesh();

  void Clear();

  [[nodiscard]] FMeshAsset& GetMesh(u64 id) { return m_MeshAssets.at(id); }

private:

  std::unordered_map<u64, FMeshAsset> m_MeshAssets{};
  FIdentifierRegistry m_IDRegistry{};

};


}


#endif //UNCANNYENGINE_ASSETREGISTRY_H
