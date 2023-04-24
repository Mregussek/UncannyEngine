
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
  math::Vector3f color{};
};


struct FMeshAssetData
{
  std::vector<FVertex> vertices{};
  std::vector<u32> indices{};
};


class FMeshAsset
{
public:

  FMeshAsset() = delete;
  explicit FMeshAsset(u64 id);

  void LoadObj(const char* path, b8 flipNormals);

  [[nodiscard]] const std::vector<FMeshAssetData>& GetData() const { return m_Data; }

  [[nodiscard]] u64 ID() const { return m_ID; }

private:

  std::vector<FMeshAssetData> m_Data{};
  u64 m_ID{ UUNUSED };

};


}


#endif //UNCANNYENGINE_MESHASSET_H
