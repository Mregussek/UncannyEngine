
#ifndef UNCANNYENGINE_FWINDOWGLFW_H
#define UNCANNYENGINE_FWINDOWGLFW_H


#include "window/Window.h"


namespace uncanny
{


class FWindowGLFW : public FWindow {
public:

  void init(FWindowSpecification windowSpecification) override;
  void terminate() override;

  void swapBuffersAndPollEvents() override;

  void close() override;
  [[nodiscard]] b32 isNotGoingToClose() const override;

private:

  FWindowSpecification mWindowSpecs{};
  GLFWwindow* mWindowPtr{ nullptr };

};


}

#endif //UNCANNYENGINE_FWINDOWGLFW_H
