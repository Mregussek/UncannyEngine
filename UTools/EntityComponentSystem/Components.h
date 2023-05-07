
#ifndef UNCANNYENGINE_COMPONENTS_H
#define UNCANNYENGINE_COMPONENTS_H


#include "UTools/UTypes.h"
#include <type_traits>
#include "UMath/Matrix4x4.h"
#include "UMath/Trig.h"


namespace uncanny
{


struct FEntityComponent { };


struct FRenderMeshComponent : public FEntityComponent
{
  u64 id{ UUNUSED };
  math::Vector3f position{};
  math::Vector3f rotation{};
  math::Vector3f scale{};

  [[nodiscard]] math::Matrix4x4f GetMatrix() const
  {
    return {
        math::Translation(position) *
        math::Rotation(math::radians(rotation.x), {1.f, 0.f, 0.f}) *
        math::Rotation(math::radians(rotation.y), {0.f, 1.f, 0.f}) *
        math::Rotation(math::radians(rotation.z), {0.f, 0.f, 1.f}) *
        math::Scale(scale)
    };
  }
};


template<typename T>
concept ConceptComponent = std::is_base_of_v<FEntityComponent, T>;


}


#endif //UNCANNYENGINE_COMPONENTS_H
