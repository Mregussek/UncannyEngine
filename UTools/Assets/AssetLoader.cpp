
#include "AssetLoader.h"
#include "MeshAsset.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "UTools/Logger/Log.h"
#include <span>


namespace uncanny
{


void FAssetLoader::LoadOBJ(const char* path, FMeshAsset* pMesh)
{
  Assimp::Importer importer;
  const aiScene* aiScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (not aiScene or
      aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE or
      not aiScene->HasMeshes() or
      not aiScene->mRootNode)
  {
    UERROR("Cannot load obj file {}\n", importer.GetErrorString());
    return;
  }

  FMeshAssetData& meshData = pMesh->GetDataPtr()->emplace_back();

  u32 verticesCount = 0;
  u32 indicesCount = 0;
  for (u32 i = 0; i < aiScene->mNumMeshes; i++)
  {
    verticesCount += aiScene->mMeshes[i]->mNumVertices;
    indicesCount += aiScene->mMeshes[i]->mNumFaces * 3;
  }
  meshData.vertices.reserve(verticesCount);
  meshData.indices.reserve(indicesCount);

  for (u32 i = 0; i < aiScene->mNumMeshes; i++)
  {
    aiMesh* aiMesh = aiScene->mMeshes[i];

    for (u32 ii = 0; ii < aiMesh->mNumVertices; ii++)
    {
      aiVector3D aiVertex = aiMesh->mVertices[ii];
      meshData.vertices.emplace_back(FVertex{ .position = { .x = aiVertex.x,
                                                            .y = aiVertex.y,
                                                            .z = aiVertex.z }});
    }

    u32 indicesIncreaseSize = meshData.indices.size();

    for (u32 ii = 0; ii < aiMesh->mNumFaces; ii++)
    {
      aiFace face = aiMesh->mFaces[ii];
      assert(face.mNumIndices == 3);
      for (u32 kk = 0; kk < face.mNumIndices; kk++)
      {
        meshData.indices.emplace_back(indicesIncreaseSize + face.mIndices[kk]);
      }
    }
  }

  importer.FreeScene();
}


}
