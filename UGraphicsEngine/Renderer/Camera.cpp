
#include "Camera.h"
#include "UMath/Trig.h"


namespace uncanny
{


static void updateCameraVectors(const FPerspectiveCameraSpecification& specs, math::Vector3f* pOutFront,
                                math::Vector3f* pOutRight, math::Vector3f* pOutUp);


void FPerspectiveCamera::Initialize(const FPerspectiveCameraSpecification& specification)
{
  m_Specification = specification;
  updateCameraVectors(m_Specification, &m_Specification.front, &m_Right, &m_Up);
}


math::Matrix4x4f FPerspectiveCamera::GetView() const
{
  return math::LookAt(m_Specification.position,
                      m_Specification.position + m_Specification.front,
                      m_Up);
}

math::Matrix4x4f FPerspectiveCamera::GetProjection() const
{
  return math::Perspective(m_Specification.fieldOfView,
                           m_Specification.aspectRatio,
                           m_Specification.near,
                           m_Specification.far);
}


FPerspectiveCameraUniformData FPerspectiveCamera::GetUniformData() const
{
  return { .inversePerspective = math::Inverse(GetProjection()),
           .inverseView = math::Inverse(GetView()) };
}


void updateCameraVectors(const FPerspectiveCameraSpecification& specs, math::Vector3f* pOutFront,
                         math::Vector3f* pOutRight, math::Vector3f* pOutUp)
{
  f32 yaw{ specs.yaw };
  f32 pitch{ specs.pitch };
  math::Vector3f front{
    .x = cos(math::radians(yaw)) * cos(math::radians(pitch)),
    .y = sin(math::radians(pitch)),
    .z = sin(math::radians(yaw)) * cos(math::radians(pitch))
  };
  *pOutFront = math::Normalize(front);
  *pOutRight = math::Normalize(math::CrossProduct(*pOutFront, specs.worldUp));
  *pOutUp = math::Normalize(math::CrossProduct(*pOutRight, *pOutFront));
}


}
