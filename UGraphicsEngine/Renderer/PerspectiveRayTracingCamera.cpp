
#include "PerspectiveRayTracingCamera.h"
#include "UMath/Trig.h"
#include "UTools/RandomGenerator.h"


namespace uncanny
{

void FPerspectiveRayTracingCamera::ProcessMovement(IWindow* pWindow, f32 deltaTime)
{
  FPerspectiveCamera::ProcessMovement(pWindow, deltaTime);

  if (HasMoved())
  {
    m_NotMovingCameraFrameCounter = 0;
    return;
  }

  if (m_NotMovingCameraFrameCounter < m_RayTracingSpecification.maxFrameCounterLimit)
  {
    m_NotMovingCameraFrameCounter += 1;
  }
}


void FPerspectiveRayTracingCamera::ContinueAccumulatingPreviousColors()
{
  m_RayTracingSpecification.accumulatePreviousColors = UTRUE;
}


void FPerspectiveRayTracingCamera::ResetAccumulatedFrameCounter()
{
  m_NotMovingCameraFrameCounter = 0;
}


void FPerspectiveRayTracingCamera::DontAccumulatePreviousColors()
{
  m_RayTracingSpecification.accumulatePreviousColors = UFALSE;
}


void FPerspectiveRayTracingCamera::SetRayTracingSpecification(FCameraRayTracingSpecification rayTracingSpecification)
{
  m_RayTracingSpecification = rayTracingSpecification;
}


FPerspectiveRayTracingCameraUniformData FPerspectiveRayTracingCamera::GetUniformData() const
{
  return {
    .inversePerspective = math::Inverse(GetProjection()),
    .inverseView = math::Inverse(GetView()),
    .randomSeed = FRandomGenerator::GetSeed(),
    .notMovingCameraFrameCount = m_NotMovingCameraFrameCounter,
    .maxRayBounces = m_RayTracingSpecification.maxRayBounces,
    .maxSamplesPerPixel = m_RayTracingSpecification.maxSamplesPerPixel,
    .accumulateColor = m_RayTracingSpecification.accumulatePreviousColors
  };
}


}
