
#ifndef UNCANNYENGINE_PERSPECTIVERAYTRACINGCAMERA_H
#define UNCANNYENGINE_PERSPECTIVERAYTRACINGCAMERA_H


#include "PerspectiveCamera.h"


namespace uncanny
{


struct FCameraRayTracingSpecification
{
  u32 maxFrameCounterLimit{ 2048 };
  u32 maxRayBounces{ 2 };
  u32 maxSamplesPerPixel{ 3 };
  i32 accumulatePreviousColors{ UTRUE };
};


struct FPerspectiveRayTracingCameraUniformData
{
  math::Matrix4x4f inversePerspective{};
  math::Matrix4x4f inverseView{};
  u32 randomSeed{ 0 };

  u32 notMovingCameraFrameCount{ 0 };

  u32 maxRayBounces{ 0 };

  u32 maxSamplesPerPixel{ 0 };

  i32 accumulateColor{ UFALSE };
  
};


class FPerspectiveRayTracingCamera : public FPerspectiveCamera
{
public:

  void ProcessMovement(IWindow* pWindow, f32 deltaTime);

  void ContinueAccumulatingPreviousColors();
  void ResetAccumulatedFrameCounter();
  void DontAccumulatePreviousColors();

  void SetRayTracingSpecification(FCameraRayTracingSpecification rayTracingSpecification);

// Getters
public:

  [[nodiscard]] FPerspectiveRayTracingCameraUniformData GetUniformData() const;

  [[nodiscard]] FCameraRayTracingSpecification& GetRayTracingSpecification() { return m_RayTracingSpecification; }

  [[nodiscard]] u32 GetAccumulatedFramesCounter() const { return m_NotMovingCameraFrameCounter; }

// Members
private:

  FCameraRayTracingSpecification m_RayTracingSpecification{};
  u32 m_NotMovingCameraFrameCounter{ 0 };

};


}


#endif //UNCANNYENGINE_PERSPECTIVERAYTRACINGCAMERA_H
