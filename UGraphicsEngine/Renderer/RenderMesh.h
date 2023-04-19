
#ifndef UNCANNYENGINE_RENDERMESH_H
#define UNCANNYENGINE_RENDERMESH_H


#include "UTools/UTypes.h"
#include "UMath/Vector3.h"
#include <vector>


namespace uncanny
{


struct FRenderVertex
{
  math::Vector3f position{};
};


struct FRenderMesh
{
  std::vector<FRenderVertex> vertices{};
  std::vector<u32> indices{};
};


class FMeshAsset;


class FRenderMeshFactory
{
public:

  static FRenderMesh CreateTriangle();

  static FRenderMesh ConvertAssetToOneRenderMesh(const FMeshAsset* pMeshAsset);

};


}


#endif //UNCANNYENGINE_RENDERMESH_H
