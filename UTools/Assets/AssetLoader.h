
#ifndef UNCANNYENGINE_ASSETLOADER_H
#define UNCANNYENGINE_ASSETLOADER_H


namespace uncanny
{


class FMeshAsset;


class FAssetLoader
{
public:

  static void LoadOBJ(const char* path, FMeshAsset* pMesh);

};


}


#endif //UNCANNYENGINE_ASSETLOADER_H
