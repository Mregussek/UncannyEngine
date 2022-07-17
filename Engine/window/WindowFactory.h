
#ifndef UNCANNYENGINE_WINDOWFACTORY_H
#define UNCANNYENGINE_WINDOWFACTORY_H


#include "Window.h"
#include "glfw/WindowGLFW.h"


namespace uncanny
{


class FWindowFactory {
public:

  template<EWindowLibrary TWindowLibrary>
  FWindow* create() {
    if constexpr (TWindowLibrary == EWindowLibrary::GLFW) {
      return &mWindowGLFW;
    }
  }

private:

  FWindowGLFW mWindowGLFW{};

};


}


#endif //UNCANNYENGINE_WINDOWFACTORY_H
