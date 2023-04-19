
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


namespace uncanny
{


struct GLFWwindowDeleter
{
  void operator()(GLFWwindow* pPointer)
  {
    glfwDestroyWindow(pPointer);
  }
};


class FWindowGLFW : public IWindow
{
public:

  void Create(const FWindowConfiguration& windowConfiguration) override;
  void Destroy() override;

  void UpdateState() override;
  void PollEvents() const override;

  void Close() override;

  [[nodiscard]] b32 IsGoingToClose() const override;

  [[nodiscard]] b32 IsMinimized() const override;

  [[nodiscard]] f32 GetDeltaTime() const override { return m_DeltaTime; }

  [[nodiscard]] FWindowSize GetSize() const override { return m_Configuration.size; }
  [[nodiscard]] FMousePosition GetMousePosition() const override { return m_MousePosition; }
  [[nodiscard]] FMouseButtonsPressed GetMouseButtonsPressed() const override { return m_MouseButtonsPressed; }
  [[nodiscard]] FMouseScrollPosition GetMouseScrollPosition() const override { return m_MouseScrollPosition; }
  [[nodiscard]] const FKeyboardButtonsPressed& GetKeyboardButtonsPressed() const override
  {
    return m_KeyboardButtonsPressed;
  }

#ifdef WIN32
  [[nodiscard]] HWND GetWin32Handle() const override;
#endif

private:

  FWindowConfiguration m_Configuration{};
  FKeyboardButtonsPressed m_KeyboardButtonsPressed{};
  FMouseButtonsPressed m_MouseButtonsPressed{};
  FMousePosition m_MousePosition{};
  FMouseScrollPosition m_MouseScrollPosition{};
  std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_pWindow{};
  f32 m_DeltaTime{ 0.0f };
  f32 m_LastFrame{ 0.0f };

};


}


#endif //UNCANNYENGINE_WINDOWGLFW_H
