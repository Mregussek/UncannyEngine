
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


void FPerspectiveCamera::ProcessKeyboardInput(IWindow* pWindow, uncanny::f32 deltaTime)
{
  const FKeyboardButtonsPressed& keyboardPressed = pWindow->GetKeyboardButtonsPressed();
  f32 velocity{ m_Specification.movementSpeed * deltaTime };
  if (keyboardPressed.w)
  {
    m_Specification.position = m_Specification.position +  m_Specification.front * velocity;
  }
  else if (keyboardPressed.s)
  {
    m_Specification.position =  m_Specification.position - m_Specification.front * velocity;
  }
  if (keyboardPressed.a)
  {
    m_Specification.position = m_Specification.position - m_Right * velocity;
  }
  else if (keyboardPressed.d)
  {
    m_Specification.position = m_Specification.position + m_Right * velocity;
  }
}


void FPerspectiveCamera::ProcessMouseMovement(IWindow* pWindow, f32 deltaTime)
{
  if (not pWindow->GetMouseButtonsPressed().right)
  {
    m_FirstMouseMove = UTRUE;
    return;
  }

  FMousePosition currentMousePosition = pWindow->GetMousePosition();

  if (m_FirstMouseMove)
  {
    m_LastMousePos = currentMousePosition;
    m_FirstMouseMove = UFALSE;
  }

  f64 xoffset{ currentMousePosition.x - m_LastMousePos.x };
  f64 yoffset{ m_LastMousePos.y - currentMousePosition.y };

  m_LastMousePos = currentMousePosition;

  xoffset *= m_Specification.sensitivity;
  yoffset *= m_Specification.sensitivity;

  m_Specification.yaw += (f32)xoffset;
  m_Specification.pitch += (f32)yoffset;

  if (m_Specification.constrainPitch) {
    if (m_Specification.pitch > 89.0f)
    {
      m_Specification.pitch = 89.0f;
    }
    if (m_Specification.pitch < -89.0f)
    {
      m_Specification.pitch = -89.0f;
    }
  }

  updateCameraVectors(m_Specification, &m_Specification.front, &m_Right, &m_Up);
}


void FPerspectiveCamera::ProcessMouseScroll(IWindow* pWindow, f32 deltaTime)
{
  FMouseScrollPosition scrollPosition = pWindow->GetMouseScrollPosition();
  m_Specification.zoom -= (f32)scrollPosition.y;
  if (m_Specification.zoom < 1.0f) {
    m_Specification.zoom = 1.0f;
  }
  else if (m_Specification.zoom > 45.0f) {
    m_Specification.zoom = 45.0f;
  }
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
