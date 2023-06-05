
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


struct FCameraRayTracingSpecification
{
  u32 maxFrameCounterLimit{ 2048 };
  u32 maxRayBounces{ 2 };
  u32 maxSamplesPerPixel{ 3 };
  i32 accumulatePreviousColors{ UTRUE };
};


struct FPerspectiveCameraUniformData
{
  math::Matrix4x4f inversePerspective{};
  math::Matrix4x4f inverseView{};
  u32 randomSeed{ 0 };
  u32 notMovingCameraFrameCount{ 0 };
  u32 maxRayBounces{ 0 };
  u32 maxSamplesPerPixel{ 0 };
  i32 accumulateColor{ UFALSE };
};


class FPerspectiveCamera
{
public:

  void Initialize(const FPerspectiveCameraSpecification& specification);

  void ProcessMovement(IWindow* pWindow, f32 deltaTime);

  void ContinueAccumulatingPreviousColors();
  void ResetAccumulatedFrameCounter();
  void DontAccumulatePreviousColors();
  [[nodiscard]] b32 ShouldAccumulatePreviousColors() const { return m_RayTracingSpecification.accumulatePreviousColors; }

  void SetRayTracingSpecification(FCameraRayTracingSpecification rayTracingSpecification);
  void SetAspectRatio(f32 aspectRatio);

  [[nodiscard]] math::Matrix4x4f GetView() const;
  [[nodiscard]] math::Matrix4x4f GetProjection() const;
  [[nodiscard]] FPerspectiveCameraUniformData GetUniformData() const;
  [[nodiscard]] b32 HasMoved() const;
  [[nodiscard]] u32 GetAccumulatedFramesCounter() const { return m_NotMovingCameraFrameCounter; }

  [[nodiscard]] FPerspectiveCameraSpecification& GetSpecification() { return m_Specification; }
  [[nodiscard]] FCameraRayTracingSpecification& GetRayTracingSpecification() { return m_RayTracingSpecification; }

private:

  void ProcessKeyboardInput(IWindow* pWindow, f32 deltaTime);
  void ProcessMouseMovement(IWindow* pWindow, f32 deltaTime);

private:

  FPerspectiveCameraSpecification m_Specification{};
  FCameraRayTracingSpecification m_RayTracingSpecification{};

  math::Vector3f m_Up{};
  math::Vector3f m_Right{};
  FMousePosition m_LastMousePos{};
  u32 m_NotMovingCameraFrameCounter{ 0 };

  b8 m_FirstMouseMove{ UFALSE };
  b8 m_IsKeyboardPressed{ UFALSE };
  b8 m_IsMousePressed{ UFALSE };

};


}


#endif //UNCANNYENGINE_PERSPECTIVECAMERA_H
