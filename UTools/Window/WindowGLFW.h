
#ifndef UNCANNYENGINE_WINDOWGLFW_H
#define UNCANNYENGINE_WINDOWGLFW_H


#ifdef WIN32
  #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#undef APIENTRY
#include <memory>
#include "IWindow.h"


namespace uncanny {


struct GLFWwindowDeleter {
  void operator()(GLFWwindow* pPointer) {
    glfwDestroyWindow(pPointer);
  }
};


class FWindowGLFW : public IWindow {
public:

  ~FWindowGLFW() override;

  void Create(const FWindowConfiguration& windowConfiguration) override;
  void Destroy() override;

  void UpdateSize() override;
  void PollEvents() const override;

  void Close() override;
  [[nodiscard]] b32 IsGoingToClose() const override;

  [[nodiscard]] b32 IsMinimized() const override;

#ifdef WIN32
  [[nodiscard]] HWND GetWin32Handle() const override;
#endif

private:

  FWindowConfiguration m_Configuration;
  std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_pWindow;

};


}


#endif //UNCANNYENGINE_WINDOWGLFW_H
