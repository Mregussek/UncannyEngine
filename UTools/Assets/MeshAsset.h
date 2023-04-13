
#ifndef UNCANNYENGINE_MESHASSET_H
#define UNCANNYENGINE_MESHASSET_H


#include "UTools/UTypes.h"


namespace uncanny
{


class FMeshAsset
{
public:

  FMeshAsset() = delete;
  explicit FMeshAsset(u64 id);

  [[nodiscard]] u64 ID() const { return m_ID; }

private:

  u64 m_ID{ UUNUSED };

};


}


#endif //UNCANNYENGINE_MESHASSET_H
