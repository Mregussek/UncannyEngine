
#ifndef UNCANNYENGINE_CAMERA_H
#define UNCANNYENGINE_CAMERA_H


#include <math/mat4.h>
#include <math/vec3.h>
#undef near
#undef far


namespace uncanny
{


enum class ECameraMovement {
  NONE, FORWARD, BACKWARD, LEFT, RIGHT
};


struct FCameraSpecification {
  vec3 position{};
  vec3 front{};
  vec3 worldUp{};
  f32 fieldOfView{ 0.f };
  f32 aspectRatio{ 0.f };
  f32 near{ 0.f };
  f32 far{ 0.f };
  f32 yaw{ 0.f };
  f32 pitch{ 0.f };
  f32 movementSpeed{ 0.f };
  f32 sensitivity{ 0.f };
  f32 zoom{ 0.f };
};


class FCamera {
public:

  void init(const FCameraSpecification& specs);

  [[nodiscard]] mat4 retrieveMatrixMVP() const;

private:

  FCameraSpecification mSpecs{};
  vec3 mUp{};
  vec3 mRight{};

};


}


#endif //UNCANNYENGINE_CAMERA_H
