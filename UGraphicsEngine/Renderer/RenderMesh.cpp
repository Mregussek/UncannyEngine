
#include "RenderMesh.h"


namespace uncanny
{


FRenderMesh FRenderMeshFactory::CreateTriangle()
{
  return {
    .vertices = {
        { .position = { .x = 1.f, .y = 1.f, .z = 0.f } },
        { .position = { .x = -1.f, .y = 1.f, .z = 0.f } },
        { .position = { .x = 0.f, .y = -1.f, .z = 0.f } },
    },
    .indices = { 0, 1, 2 }
  };
}


}
