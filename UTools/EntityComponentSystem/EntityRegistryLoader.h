
#ifndef UNCANNYENGINE_ENTITYLOADER_H
#define UNCANNYENGINE_ENTITYLOADER_H


namespace uncanny
{


class FEntityRegistry;
class FAssetRegistry;


class FEntityRegistryLoader
{
public:

  /// @details I have decided to write a FEntityRegistryLoader and pass a asset registry pointer as every
  /// FRenderMeshComponent contains an ID to asset registry mesh. During loading all entities we need to
  /// load mesh and generate its unique ID.
  /// TODO: this is not ideal, I would like to write some FAssetRegistryLoader which will load meshes with already
  ///     generated IDs and entity registry will also load those already generated IDs, then we would have complete
  ///     separation between entity registry and asset registry
  static void LoadJsonScene(const char* path, FEntityRegistry* pEntityRegistry, FAssetRegistry* pAssetRegistry);

};


}


#endif //UNCANNYENGINE_ENTITYLOADER_H
