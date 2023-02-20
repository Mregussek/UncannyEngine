
#ifndef UNCANNYENGINE_WINDOWGLFW_H
#define UNCANNYENGINE_WINDOWGLFW_H


#ifdef WIN32
  #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#undef APIENTRY
#include <memory>
#include "UTools/UTypes.h"


namespace uncanny {


struct FWindowConfiguration {
  u32 resizable{ UFALSE };
  b32 fullscreen{ UFALSE };
  i32 width{ 0 };
  i32 height{ 0 };
  const char* name{ "" };
};


struct GLFWwindowDeleter {
  void operator()(GLFWwindow* pPointer) {
    glfwDestroyWindow(pPointer);
  }
};


class FWindowGLFW {
public:

  ~FWindowGLFW();

  void Create(const FWindowConfiguration& windowConfiguration);
  void Destroy();

  void UpdateSize();
  void PollEvents() const;

  void Close();
  [[nodiscard]] b32 IsGoingToClose() const;

  [[nodiscard]] b32 IsMinimized() const;

#ifdef WIN32
  [[nodiscard]] HWND GetWin32Handle() const;
#endif

private:

  FWindowConfiguration m_Configuration;
  std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_pWindow;

};


}


#endif //UNCANNYENGINE_WINDOWGLFW_H
