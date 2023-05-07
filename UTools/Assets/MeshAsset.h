
#ifndef UNCANNYENGINE_MESHASSET_H
#define UNCANNYENGINE_MESHASSET_H


#include "UTools/UTypes.h"
#include "UMath/Vector3.h"
#include <vector>


namespace uncanny
{


struct FVertex
{
  math::Vector3f position{};
  math::Vector3f normal{};
};


struct FMeshAssetData
{
  std::vector<FVertex> vertices{};
  std::vector<u32> indices{};
  u32 materialIndex{ UUNUSED };
};


struct FMaterialData
{
  math::Vector3f ambient{};
  math::Vector3f diffuse{};
  math::Vector3f specular{};
  math::Vector3f emissive{};
  math::Vector3f transparent{};
  f32 shininess{ 0.f };
  f32 opacity{ 0.f };
  f32 indexOfRefraction{ 0.f };
  i32 illuminationModel{ 0 };
};


class FMeshAsset
{
public:

  FMeshAsset() = delete;
  explicit FMeshAsset(u64 id);

  void LoadObj(const char* path, b8 flipNormals);

  [[nodiscard]] const std::vector<FMeshAssetData>& GetMeshes() const { return m_Meshes; }
  [[nodiscard]] const std::vector<FMaterialData>& GetMaterials() const { return m_Materials; }

  [[nodiscard]] u64 ID() const { return m_ID; }

private:

  std::vector<FMeshAssetData> m_Meshes{};
  std::vector<FMaterialData> m_Materials{};
  u64 m_ID{ UUNUSED };

};


}


#endif //UNCANNYENGINE_MESHASSET_H
