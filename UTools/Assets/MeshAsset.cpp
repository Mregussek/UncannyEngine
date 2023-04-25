
#include "MeshAsset.h"
#include "AssetLoader.h"


namespace uncanny
{


FMeshAsset::FMeshAsset(u64 id)
  : m_ID(id)
{
}


void FMeshAsset::LoadObj(const char* path, b8 flipNormals)
{
  FAssetLoader::LoadOBJ(path, &m_Meshes, &m_Materials, flipNormals);
}


}
