
#ifndef UNCANNYENGINE_PERSPECTIVECAMERA_H
#define UNCANNYENGINE_PERSPECTIVECAMERA_H


#include "UMath/Matrix4x4.h"
#include "UTools/Window/IWindow.h"
#undef near
#undef far


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
  b8 constrainPitch{ UFALSE };
};


class FPerspectiveCamera
{
public:

  void Initialize(const FPerspectiveCameraSpecification& specification);

  void ProcessMovement(IWindow* pWindow, f32 deltaTime);

  void ResetSpecification();

  [[nodiscard]] b32 HasMoved() const;

  void SetAspectRatio(f32 aspectRatio);

// Getters
public:

  [[nodiscard]] math::Matrix4x4f GetView() const;
  [[nodiscard]] math::Matrix4x4f GetProjection() const;
  [[nodiscard]] FPerspectiveCameraSpecification& GetSpecification() { return m_Specification; } 

// Private methods
private:

  void ProcessKeyboardInput(IWindow* pWindow, f32 deltaTime);
  void ProcessMouseMovement(IWindow* pWindow, f32 deltaTime);

// Members
private:

  FPerspectiveCameraSpecification m_FirstSpecification{};
  FPerspectiveCameraSpecification m_Specification{};

  FMousePosition m_LastMousePos{};
  math::Vector3f m_Up{};
  math::Vector3f m_Right{};

  b8 m_FirstMouseMove{ UFALSE };
  b8 m_IsKeyboardPressed{ UFALSE };
  b8 m_IsMousePressed{ UFALSE };

};


}


#endif //UNCANNYENGINE_PERSPECTIVECAMERA_H
