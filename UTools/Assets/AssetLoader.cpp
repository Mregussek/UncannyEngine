
#include "AssetLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "UTools/Logger/Log.h"


namespace uncanny
{


class FAssimpSceneProcessor
{
public:

  FAssimpSceneProcessor(std::vector<FMeshAssetData>* pReturnMeshData,
                        std::vector<FMaterialData>* pReturnMaterialData)
    : m_pReturnMeshData(pReturnMeshData),
    m_pReturnMaterialData(pReturnMaterialData)
  {
  }

  void ProcessMaterial(const aiScene* pScene)
  {
    m_pReturnMaterialData->reserve(pScene->mNumMaterials);
    for (u32 i = 0; i < pScene->mNumMaterials; i++)
    {
      aiMaterial* aiMat = pScene->mMaterials[i];

      aiColor3D ambient;
      aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

      aiColor3D diffuse;
      aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

      aiColor3D specular;
      aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specular);

      f32 shininess{ 0.f };
      aiMat->Get(AI_MATKEY_SHININESS, shininess);

      f32 shininessStrength{ 0.f };
      aiMat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);

      aiColor3D transparent;
      aiMat->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);

      f32 transparencyFactor{ 0.f };
      aiMat->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparencyFactor);

      aiColor3D reflective;
      aiMat->Get(AI_MATKEY_COLOR_REFLECTIVE, reflective);

      f32 reflectivity{ 0.f };
      aiMat->Get(AI_MATKEY_REFLECTIVITY, reflectivity);

      aiColor3D emissive;
      aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);

      f32 indexOfRefraction{ 0.f };
      aiMat->Get(AI_MATKEY_REFRACTI, indexOfRefraction);

      i32 illuminationModel = aiShadingMode_NoShading;
      aiMat->Get(AI_MATKEY_SHADING_MODEL, illuminationModel);

      m_pReturnMaterialData->push_back(FMaterialData{
        .ambient = { .x = ambient.r, .y = ambient.g, .z = ambient.b },
        .diffuse = { .x = diffuse.r, .y = diffuse.g, .z = diffuse.b },
        .specular = { .x = specular.r, .y = specular.g, .z = specular.b },
        .emissive = { .x = emissive.r, .y = emissive.g, .z = emissive.b },
        .reflective = { .x = reflective.r, .y = reflective.g, .z = reflective.b },
        .shininess = shininess,
        .reflectivity = reflectivity,
        .indexOfRefraction = indexOfRefraction,
        .illuminationModel = illuminationModel
      });
    }
  }

  void ProcessNode(aiNode* pNode, const aiScene* pScene, b8 flipNormals)
  {
    for(u32 i = 0; i < pNode->mNumMeshes; i++)
    {
      aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
      FMeshAssetData& rtnMeshData = m_pReturnMeshData->emplace_back();

      ProcessMesh(pMesh, pScene, &rtnMeshData, flipNormals);
    }

    for(u32 i = 0; i < pNode->mNumChildren; i++)
    {
      ProcessNode(pNode->mChildren[i], pScene, flipNormals);
    }
  }

private:

  static void ProcessMesh(aiMesh* pMesh, const aiScene* pScene, FMeshAssetData* pReturnData, b8 flipNormals)
  {
    pReturnData->materialIndex = pMesh->mMaterialIndex;
    aiMaterial* aiMat = pScene->mMaterials[pReturnData->materialIndex];
    aiColor3D color;
    aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

    pReturnData->vertices.reserve(pMesh->mNumVertices);
    for (u32 vertexIdx = 0; vertexIdx < pMesh->mNumVertices; vertexIdx++)
    {
      aiVector3D aiVertex = pMesh->mVertices[vertexIdx];
      aiVector3D aiNormal = pMesh->mNormals ? pMesh->mNormals[vertexIdx] : aiVector3D{ 0.f, 0.f, 0.f };

      pReturnData->vertices.push_back(
          FVertex{
            .position = {
                .x = aiVertex.x,
                .y = aiVertex.y,
                .z = aiVertex.z },
            .normal = {
                .x = aiNormal.x,
                .y = aiNormal.y,
                .z = aiNormal.z }
          });
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

    // Calculate normals
    if (pMesh->HasNormals())
    {
      return;
    }
    for (u32 idx = 0; idx < pReturnData->indices.size(); idx += 3)
    {
      const u32 vertexA = pReturnData->indices[idx];
      const u32 vertexB = pReturnData->indices[idx + 1];
      const u32 vertexC = pReturnData->indices[idx + 2];

      const math::Vector3f edgeAB = pReturnData->vertices[vertexB].position - pReturnData->vertices[vertexA].position;
      const math::Vector3f edgeAC = pReturnData->vertices[vertexC].position - pReturnData->vertices[vertexA].position;

      // The cross product is perpendicular to both input vectors (normal to the plane).
      // Flip the argument order if you need the opposite winding.
      const math::Vector3f areaWeightedNormal = flipNormals ? math::CrossProduct(edgeAC, edgeAB) :
          math::CrossProduct(edgeAB, edgeAC);

      // Don't normalize this vector just yet. Its magnitude is proportional to the
      // area of the triangle (times 2), so this helps ensure tiny/skinny triangles
      // don't have an outsized impact on the final normal per vertex.
      pReturnData->vertices[vertexA].normal = pReturnData->vertices[vertexA].normal + areaWeightedNormal;
      pReturnData->vertices[vertexB].normal = pReturnData->vertices[vertexB].normal + areaWeightedNormal;
      pReturnData->vertices[vertexC].normal = pReturnData->vertices[vertexC].normal + areaWeightedNormal;
    }
    for (FVertex& vertex : pReturnData->vertices)
    {
      vertex.normal = math::Normalize(vertex.normal);
    }
  }

private:

  std::vector<FMeshAssetData>* m_pReturnMeshData{ nullptr };
  std::vector<FMaterialData>* m_pReturnMaterialData{ nullptr };

};


void FAssetLoader::LoadOBJ(const char* path, std::vector<FMeshAssetData>* pMeshData,
                           std::vector<FMaterialData>* pMaterialData, b8 flipNormals)
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

  FAssimpSceneProcessor sceneProcessor(pMeshData, pMaterialData);
  sceneProcessor.ProcessNode(aiScene->mRootNode, aiScene, flipNormals);
  sceneProcessor.ProcessMaterial(aiScene);

  importer.FreeScene();

  UDEBUG("Loaded .obj file: {}", path);
}


}
