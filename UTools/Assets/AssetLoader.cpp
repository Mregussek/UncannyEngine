
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
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
  if (not scene or not scene->HasMeshes()) {
    UERROR("Cannot load obj file {}\n", importer.GetErrorString());
    return;
  }

  for (u32 i = 0; i < scene->mNumMeshes; i++)
  {
    FSubmeshAsset& submesh = pMesh->AppendSubmesh();
    aiMesh* pAiMesh = scene->mMeshes[i];

    submesh.positions.reserve(pAiMesh->mNumFaces * 3);
    submesh.indices.reserve(pAiMesh->mNumFaces * 3);

    for (u32 ii = 0; ii < pAiMesh->mNumFaces; ii++)
    {
      aiFace face = pAiMesh->mFaces[ii];

      submesh.indices.push_back(face.mIndices[0]);
      submesh.indices.push_back(face.mIndices[1]);
      submesh.indices.push_back(face.mIndices[2]);

      {
        aiVector3D pos = pAiMesh->mVertices[face.mIndices[0]];
        submesh.positions.emplace_back(pos.x, pos.y, pos.z);
      }
      {
        aiVector3D pos = pAiMesh->mVertices[face.mIndices[1]];
        submesh.positions.emplace_back(pos.x, pos.y, pos.z);
      }
      {
        aiVector3D pos = pAiMesh->mVertices[face.mIndices[2]];
        submesh.positions.emplace_back(pos.x, pos.y, pos.z);
      }
    }
  }

  importer.FreeScene();
}


}
