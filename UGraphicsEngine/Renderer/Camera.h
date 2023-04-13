
#ifndef UNCANNYENGINE_CAMERA_H
#define UNCANNYENGINE_CAMERA_H


#include "UMath/Matrix4x4.h"


namespace uncanny
{


struct FPerspectiveCameraSpecification
{
  math::Vector3f position{};
  math::Vector3f front{};
  math::Vector3f worldUp{};
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


struct FPerspectiveCameraUniformData
{
  math::Matrix4x4f inversePerspective{};
  math::Matrix4x4f inverseView{};
};


class FPerspectiveCamera
{
public:

  void Initialize(const FPerspectiveCameraSpecification& specification);

  [[nodiscard]] math::Matrix4x4f GetView() const;

  [[nodiscard]] math::Matrix4x4f GetProjection() const;

  [[nodiscard]] FPerspectiveCameraUniformData GetUniformData() const;

private:

  FPerspectiveCameraSpecification m_Specification{};
  math::Vector3f m_Up{};
  math::Vector3f m_Right{};

};


}


#endif //UNCANNYENGINE_CAMERA_H
