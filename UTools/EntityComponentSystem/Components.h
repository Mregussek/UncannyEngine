
#ifndef UNCANNYENGINE_COMPONENTS_H
#define UNCANNYENGINE_COMPONENTS_H


#include "UTools/UTypes.h"
#include <type_traits>


namespace uncanny
{


struct FEntityComponent { };


struct FRenderMeshComponent : public FEntityComponent
{
  u64 id{ UUNUSED };
};


template<typename T>
concept ConceptComponent = std::is_base_of_v<FEntityComponent, T>;


}


#endif //UNCANNYENGINE_COMPONENTS_H
