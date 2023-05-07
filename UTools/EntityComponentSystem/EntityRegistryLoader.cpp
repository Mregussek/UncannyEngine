
#include "EntityRegistryLoader.h"
#include "EntityRegistry.h"
#include "UTools/Assets/AssetRegistry.h"
#include "UTools/Logger/Log.h"
#include "UTools/Filesystem/Path.h"
#include <nlohmann/json.hpp>
#include <fstream>


namespace uncanny
{


void FEntityRegistryLoader::LoadJsonScene(const char* path, FEntityRegistry* pEntityRegistry,
                                          FAssetRegistry* pAssetRegistry)
{
  std::ifstream jsonFileStream{ path };
  if (not jsonFileStream.is_open())
  {
    UERROR("Cannot open json scene file {}!", path);
    return;
  }
  UDEBUG("Loading json scene file: {}", path);

  nlohmann::json data = nlohmann::json::parse(jsonFileStream);
  jsonFileStream.close();

  const FPath enginePath = FPath::GetEngineProjectPath();

  u32 entityIdx = 0;
  for (nlohmann::json& _entity : data["Scene"]["Entities"]) {
    // Load path to mesh...
    u32 pathIdx = 0;
    FPath meshAssetPath = enginePath;
    for (nlohmann::json& _meshPath : data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["path"])
    {
      std::string pathPart = data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["path"][pathIdx];
      meshAssetPath = FPath::Append(meshAssetPath, pathPart.c_str());
      pathIdx++;
    }

    math::Vector3f position{
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["position"]["x"].get<float>(),
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["position"]["y"].get<float>(),
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["position"]["z"].get<float>()
    };
    math::Vector3f rotation{
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["rotation"]["x"].get<float>(),
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["rotation"]["y"].get<float>(),
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["rotation"]["z"].get<float>()
    };
    math::Vector3f scale{
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["scale"]["x"].get<float>(),
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["scale"]["y"].get<float>(),
        data["Scene"]["Entities"][entityIdx]["RenderMeshComponent"]["scale"]["z"].get<float>()
    };

    FMeshAsset& meshAsset = pAssetRegistry->RegisterMesh();
    meshAsset.LoadObj(meshAssetPath.GetString().c_str(), UFALSE);

    FEntity entity = pEntityRegistry->Register();
    entity.Add<FRenderMeshComponent>(FRenderMeshComponent{
        .id = meshAsset.ID(),
        .position = position,
        .rotation = rotation,
        .scale = scale
    });

    entityIdx++;
  }
}


}
