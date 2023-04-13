
#ifndef UNCANNYENGINE_MESHASSET_H
#define UNCANNYENGINE_MESHASSET_H


#include "UTools/UTypes.h"
#include "UMath/Vector3.h"
#include <vector>


namespace uncanny
{


struct FSubmeshAsset
{
  std::vector<math::Vector3f> positions{};
  std::vector<u32> indices{};
};


class FMeshAsset
{
public:

  FMeshAsset() = delete;
  explicit FMeshAsset(u64 id);

  FSubmeshAsset& AppendSubmesh();

  [[nodiscard]] const std::vector<FSubmeshAsset>& GetSubmeshes() const { return m_Submeshes; }

  [[nodiscard]] u64 ID() const { return m_ID; }

private:

  std::vector<FSubmeshAsset> m_Submeshes{};
  u64 m_ID{ UUNUSED };

};


}


#endif //UNCANNYENGINE_MESHASSET_H
