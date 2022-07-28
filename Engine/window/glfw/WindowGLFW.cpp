
#include "WindowGLFW.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FWindowGLFW::init(FWindowSpecification windowSpecification) {
  UTRACE("Initializing GLFW window...");

  const i32 isGlfwInitialized{ glfwInit() };
  if (!isGlfwInitialized) {
    UFATAL("Cannot initialize GLFW library! ErrCode: {}", isGlfwInitialized);
    return;
  }
  mWindowSpecs = windowSpecification;
  mWindowPtr = glfwCreateWindow(mWindowSpecs.width, mWindowSpecs.height, mWindowSpecs.name, nullptr,
                                nullptr);
  if (!mWindowPtr) {
    UFATAL("Cannot initialize GLFW window!");
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(mWindowPtr);
  glClearColor( 0.4f, 0.3f, 0.4f, 0.0f );
  UINFO("Initialized GLFW window! name: {} width: {} height: {}",
        mWindowSpecs.name, mWindowSpecs.width, mWindowSpecs.height);
}


void FWindowGLFW::terminate() {
  UINFO("Terminating GLFW window! name: {}", mWindowSpecs.name);
  glfwTerminate();
}


void FWindowGLFW::swapBuffersAndPollEvents() {
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(mWindowPtr);
  glfwPollEvents();
}


void FWindowGLFW::close() {
  UINFO("Closing GLFW window! name: {}", mWindowSpecs.name);
  glfwSetWindowShouldClose(mWindowPtr, GLFW_TRUE);
}


b32 FWindowGLFW::isNotGoingToClose() const {
  return !glfwWindowShouldClose(mWindowPtr);
}


EWindowLibrary FWindowGLFW::getLibrary() const {
  return EWindowLibrary::GLFW;
}


FWindowSpecification FWindowGLFW::getSpecs() const {
  return mWindowSpecs;
}


}
