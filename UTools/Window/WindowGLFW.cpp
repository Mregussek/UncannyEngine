
#include "WindowGLFW.h"
#include "UTools/Logger/Log.h"


namespace uncanny
{


static FMousePosition* g_pMousePosition{ nullptr };
static FMouseButtonsPressed* g_pMouseButtonsPressed{ nullptr };
static FKeyboardButtonsPressed* g_pKeyboardButtonsPressed{ nullptr };


static void ErrorCallback(i32 error, const char* description)
{
  UERROR("GLFW Error {}: {}", error, description);
}


static void CursorPosCallback(GLFWwindow* pWindow, f64 x, f64 y)
{
  FWindowSize windowSize{};
  glfwGetFramebufferSize(pWindow, &windowSize.width, &windowSize.height);
  g_pMousePosition->x = x / static_cast<f64>(windowSize.width);
  g_pMousePosition->y = y / static_cast<f64>(windowSize.height);
}


static void MouseButtonCallback(GLFWwindow* pWindow, i32 button, i32 action, i32 mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    g_pMouseButtonsPressed->left = action == GLFW_PRESS;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    g_pMouseButtonsPressed->right = action == GLFW_PRESS;
  }
}


static void KeyboardButtonCallback(GLFWwindow* pWindow, i32 key, i32 scancode, i32 action, i32 mods)
{
  const b32 press = action != GLFW_RELEASE;
  auto SetPressForKey = [press, key](b32& writeKey, i32 glfwKey)
  {
    if (key == glfwKey)
    {
      writeKey = press;
    }
  };
  SetPressForKey(g_pKeyboardButtonsPressed->escape, GLFW_KEY_ESCAPE);
  SetPressForKey(g_pKeyboardButtonsPressed->w, GLFW_KEY_W);
  SetPressForKey(g_pKeyboardButtonsPressed->s, GLFW_KEY_S);
  SetPressForKey(g_pKeyboardButtonsPressed->a, GLFW_KEY_A);
  SetPressForKey(g_pKeyboardButtonsPressed->d, GLFW_KEY_D);
  SetPressForKey(g_pKeyboardButtonsPressed->num1, GLFW_KEY_1);
  SetPressForKey(g_pKeyboardButtonsPressed->num2, GLFW_KEY_2);
  SetPressForKey(g_pKeyboardButtonsPressed->leftShift, GLFW_KEY_LEFT_SHIFT);
  SetPressForKey(g_pKeyboardButtonsPressed->leftControl, GLFW_KEY_LEFT_CONTROL);
  SetPressForKey(g_pKeyboardButtonsPressed->space, GLFW_KEY_SPACE);
}



void FWindowGLFW::Create(const FWindowConfiguration &windowConfiguration)
{
  m_Configuration = windowConfiguration;

  glfwSetErrorCallback(ErrorCallback);

  i32 result = glfwInit();
  if (!result)
  {
    const char* msg{ "Cannot initialize glfw library!" };
    UCRITICAL(msg);
    throw std::runtime_error(msg);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, m_Configuration.resizable ? GLFW_TRUE : GLFW_FALSE);

  m_pWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfwCreateWindow(m_Configuration.size.width,
                                                                              m_Configuration.size.height,
                                                                              m_Configuration.name,
                                                                              nullptr,
                                                                              nullptr));

  g_pMousePosition = &m_MousePosition;
  g_pMouseButtonsPressed = &m_MouseButtonsPressed;
  g_pKeyboardButtonsPressed = &m_KeyboardButtonsPressed;

  glfwSetCursorPosCallback(m_pWindow.get(), CursorPosCallback);
  glfwSetMouseButtonCallback(m_pWindow.get(), MouseButtonCallback);
  glfwSetKeyCallback(m_pWindow.get(), KeyboardButtonCallback);
  glfwSetInputMode(m_pWindow.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  UpdateState();

  UDEBUG("Created window!");
}


void FWindowGLFW::Destroy()
{
  m_pWindow.reset();
  glfwTerminate();
}


void FWindowGLFW::UpdateState()
{
  glfwGetFramebufferSize(m_pWindow.get(), &m_Configuration.size.width,
                         &m_Configuration.size.height);

  f32 currentFrame{ (f32)glfwGetTime() };
  m_DeltaTime = currentFrame - m_LastFrame;
  m_LastFrame = currentFrame;
}


void FWindowGLFW::PollEvents() const
{
  glfwPollEvents();
}


void FWindowGLFW::Close()
{
  glfwSetWindowShouldClose(m_pWindow.get(), GLFW_TRUE);
}


b32 FWindowGLFW::IsGoingToClose() const
{
  return glfwWindowShouldClose(m_pWindow.get());
}


b32 FWindowGLFW::IsMinimized() const
{
  if (m_Configuration.size.width == 0 and m_Configuration.size.height == 0)
  {
    return UTRUE;
  }
  return UFALSE;
}


#ifdef WIN32
HWND FWindowGLFW::GetWin32Handle() const
{
  return glfwGetWin32Window(m_pWindow.get());
}
#endif


}
