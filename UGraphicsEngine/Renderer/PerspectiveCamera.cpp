
#include "PerspectiveCamera.h"
#include "UMath/Trig.h"
#include "UTools/RandomGenerator.h"


namespace uncanny
{


static void updateCameraVectors(f32 yaw, f32 pitch, math::Vector3f worldUp, math::Vector3f* pOutFront,
                                math::Vector3f* pOutRight, math::Vector3f* pOutUp);


void FPerspectiveCamera::Initialize(const FPerspectiveCameraSpecification& specification)
{
  m_Specification = specification;
  m_FirstSpecification = specification;
  updateCameraVectors(m_Specification.yaw, m_Specification.pitch, m_Specification.worldUp, &m_Specification.front,
                      &m_Right, &m_Up);
}


void FPerspectiveCamera::ResetSpecification()
{
  m_Specification = m_FirstSpecification;
  updateCameraVectors(m_Specification.yaw, m_Specification.pitch, m_Specification.worldUp, &m_Specification.front,
                      &m_Right, &m_Up);
}


void FPerspectiveCamera::SetAspectRatio(f32 aspectRatio)
{
  m_Specification.aspectRatio = aspectRatio;
}


void FPerspectiveCamera::ProcessKeyboardInput(IWindow* pWindow, uncanny::f32 deltaTime)
{
  m_IsKeyboardPressed = UFALSE;

  const FKeyboardButtonsPressed& keyboardPressed = pWindow->GetKeyboardButtonsPressed();
  f32 velocity{ m_Specification.movementSpeed * deltaTime };

  if (keyboardPressed.w)
  {
    m_Specification.position = m_Specification.position +  m_Specification.front * velocity;
    m_IsKeyboardPressed = UTRUE;
  }
  else if (keyboardPressed.s)
  {
    m_Specification.position =  m_Specification.position - m_Specification.front * velocity;
    m_IsKeyboardPressed = UTRUE;
  }
  if (keyboardPressed.a)
  {
    m_Specification.position = m_Specification.position - m_Right * velocity;
    m_IsKeyboardPressed = UTRUE;
  }
  else if (keyboardPressed.d)
  {
    m_Specification.position = m_Specification.position + m_Right * velocity;
    m_IsKeyboardPressed = UTRUE;
  }
}


void FPerspectiveCamera::ProcessMouseMovement(IWindow* pWindow, f32 deltaTime)
{
  m_IsMousePressed = UFALSE;
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

  updateCameraVectors(m_Specification.yaw, m_Specification.pitch, m_Specification.worldUp, &m_Specification.front,
                      &m_Right, &m_Up);
  m_IsMousePressed = UTRUE;
}


void FPerspectiveCamera::ProcessMovement(IWindow* pWindow, f32 deltaTime)
{
  ProcessKeyboardInput(pWindow, deltaTime);
  ProcessMouseMovement(pWindow, deltaTime);
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


b32 FPerspectiveCamera::HasMoved() const
{
  return m_IsKeyboardPressed or m_IsMousePressed;
}



void updateCameraVectors(f32 yaw, f32 pitch, math::Vector3f worldUp, math::Vector3f* pOutFront,
                         math::Vector3f* pOutRight, math::Vector3f* pOutUp)
{
  math::Vector3f front{
      .x = cos(math::radians(yaw)) * cos(math::radians(pitch)),
      .y = sin(math::radians(pitch)),
      .z = sin(math::radians(yaw)) * cos(math::radians(pitch))
  };
  *pOutFront = math::Normalize(front);
  *pOutRight = math::Normalize(math::CrossProduct(*pOutFront, worldUp));
  *pOutUp = math::Normalize(math::CrossProduct(*pOutRight, *pOutFront));
}


}
