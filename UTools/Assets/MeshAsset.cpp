
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


void FMeshAsset::MakeReflective()
{
  for (FMaterialData& material : m_Materials)
  {
    material.diffuse = { 0.5f, 0.5f, 0.5f };
    material.illuminationModel = 8;
  }
}


}
