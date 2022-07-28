
#ifndef UNCANNYENGINE_WINDOWGLFW_H
#define UNCANNYENGINE_WINDOWGLFW_H


#include "window/Window.h"
#ifdef WIN32
  #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#undef APIENTRY


namespace uncanny
{


class FWindowGLFW : public FWindow {
public:

  void init(FWindowSpecification windowSpecification) override;
  void terminate() override;

  void swapBuffersAndPollEvents() override;

  void close() override;
  [[nodiscard]] b32 isNotGoingToClose() const override;

  [[nodiscard]] EWindowLibrary getLibrary() const override;
  [[nodiscard]] FWindowSpecification getSpecs() const override;

#ifdef WIN32
  [[nodiscard]] HWND getWindowHandle() const;
#endif

private:

  FWindowSpecification mSpecs{};
  GLFWwindow* mWindowPtr{ nullptr };

};


}

#endif //UNCANNYENGINE_WINDOWGLFW_H
