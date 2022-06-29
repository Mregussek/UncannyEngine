
#include "WindowGLFW.h"

namespace uncanny
{


void FWindowGLFW::init(FWindowSpecification windowSpecification) {
  const i32 isGlfwInitialized{ glfwInit() };
  if (!isGlfwInitialized) {
    return;
  }
  mWindowSpecs = windowSpecification;
  mWindowPtr = glfwCreateWindow(mWindowSpecs.width, mWindowSpecs.height, mWindowSpecs.name, nullptr,
                                nullptr);
  if (!mWindowPtr) {
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(mWindowPtr);
  glClearColor( 0.4f, 0.3f, 0.4f, 0.0f );
}


void FWindowGLFW::terminate() {
  glfwTerminate();
}


void FWindowGLFW::swapBuffersAndPollEvents() {
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(mWindowPtr);
  glfwPollEvents();
}


void FWindowGLFW::close() {
  glfwSetWindowShouldClose(mWindowPtr, GLFW_TRUE);
}


b32 FWindowGLFW::isNotGoingToClose() const {
  return !glfwWindowShouldClose(mWindowPtr);
}


}
