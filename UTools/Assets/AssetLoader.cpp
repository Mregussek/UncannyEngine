
#include "AssetLoader.h"
#include "MeshAsset.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "UTools/Logger/Log.h"
#include <span>


namespace uncanny
{


class FAssimpSceneProcessor
{
public:

  explicit FAssimpSceneProcessor(std::vector<FMeshAssetData>* pReturnData)
    : m_pReturnData(pReturnData)
  {
  }

  void ProcessNode(aiNode* pNode, const aiScene* pScene)
  {
    for(u32 i = 0; i < pNode->mNumMeshes; i++)
    {
      aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
      FMeshAssetData& rtnMeshData = m_pReturnData->emplace_back();

      ProcessMesh(pMesh, pScene, &rtnMeshData);
    }

    for(u32 i = 0; i < pNode->mNumChildren; i++)
    {
      ProcessNode(pNode->mChildren[i], pScene);
    }
  }

private:

  static void ProcessMesh(aiMesh* pMesh, const aiScene* pScene, FMeshAssetData* pReturnData)
  {
    pReturnData->vertices.reserve(pMesh->mNumVertices);
    for (u32 vertexIdx = 0; vertexIdx < pMesh->mNumVertices; vertexIdx++)
    {
      aiVector3D aiVertex = pMesh->mVertices[vertexIdx];
      pReturnData->vertices.push_back(FVertex{ .position = { .x = aiVertex.x,
                                                             .y = aiVertex.y,
                                                             .z = aiVertex.z }});
    }

    pReturnData->indices.reserve(pMesh->mNumFaces * 3);
    for (u32 faceIdx = 0; faceIdx < pMesh->mNumFaces; faceIdx++)
    {
      aiFace face = pMesh->mFaces[faceIdx];
      assert(face.mNumIndices == 3);
      for (u32 kk = 0; kk < face.mNumIndices; kk++)
      {
        pReturnData->indices.push_back((u32)face.mIndices[kk]);
      }
    }
  }


  std::vector<FMeshAssetData>* m_pReturnData{ nullptr };

};


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

  FAssimpSceneProcessor sceneProcessor(pMesh->GetDataPtr());
  sceneProcessor.ProcessNode(aiScene->mRootNode, aiScene);

  importer.FreeScene();
}


}
