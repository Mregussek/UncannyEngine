
#ifndef UNCANNYENGINE_ASSETLOADER_H
#define UNCANNYENGINE_ASSETLOADER_H


#include "MeshAsset.h"


namespace uncanny
{


class FMeshAssetData;


class FAssetLoader
{
public:

  static void LoadOBJ(const char* path, std::vector<FMeshAssetData>* pMeshData,
                      std::vector<FMaterialData>* pMaterialData, b8 flipNormals);

};


}


#endif //UNCANNYENGINE_ASSETLOADER_H
