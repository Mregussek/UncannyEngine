
#include "WindowGLFW.h"
#include "UTools/Logger/Log.h"


namespace uncanny {


static void errorCallbackGLFW(i32 error, const char* description) {
  UERROR("GLFW Error {}: {}", error, description);
}


FWindowGLFW::~FWindowGLFW() {
  Destroy();
}


void FWindowGLFW::Create(const FWindowConfiguration &windowConfiguration) {
  m_Configuration = windowConfiguration;

  glfwSetErrorCallback(errorCallbackGLFW);

  i32 result = glfwInit();
  if (!result) {
    const char* msg{ "Cannot initialize glfw library!" };
    UCRITICAL(msg);
    throw std::runtime_error(msg);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, m_Configuration.resizable ? GLFW_TRUE : GLFW_FALSE);

  m_pWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(glfwCreateWindow(m_Configuration.width, m_Configuration.height, m_Configuration.name, nullptr, nullptr));

  UpdateSize();
}


void FWindowGLFW::Destroy() {
  m_pWindow.reset();
  glfwTerminate();
}


void FWindowGLFW::UpdateSize() {
  glfwGetFramebufferSize(m_pWindow.get(), &m_Configuration.width, &m_Configuration.height);
}


void FWindowGLFW::PollEvents() const {
  glfwPollEvents();
}


void FWindowGLFW::Close() {
  glfwSetWindowShouldClose(m_pWindow.get(), GLFW_TRUE);
}


b32 FWindowGLFW::IsGoingToClose() const {
  return glfwWindowShouldClose(m_pWindow.get());
}


b32 FWindowGLFW::IsMinimized() const {
  return (m_Configuration.width <= 1 and m_Configuration.height <= 1);
}


#ifdef WIN32
HWND FWindowGLFW::GetWin32Handle() const {
  return glfwGetWin32Window(m_pWindow.get());
}
#endif


}
