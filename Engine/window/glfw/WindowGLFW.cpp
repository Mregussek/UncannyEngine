
#include "WindowGLFW.h"
#include <utilities/Logger.h>


namespace uncanny
{


static void errorCallbackGLFW(i32 error, const char* description) {
  UERROR("GLFW Error {}: {}", error, description);
}


void FWindowGLFW::init(FWindowSpecification windowSpecification) {
  UTRACE("Initializing GLFW window...");
  mSpecs = windowSpecification;

  glfwSetErrorCallback(errorCallbackGLFW);

  const i32 isGlfwInitialized{ glfwInit() };
  if (!isGlfwInitialized) {
    UFATAL("Cannot initialize GLFW library! ErrCode: {}", isGlfwInitialized);
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, mSpecs.config.resizable ? GLFW_TRUE : GLFW_FALSE);

  mWindowPtr = glfwCreateWindow(mSpecs.width, mSpecs.height, mSpecs.name, nullptr, nullptr);
  if (!mWindowPtr) {
    UFATAL("Cannot initialize GLFW window!");
    glfwTerminate();
    return;
  }

  updateFramebufferSize();

  UINFO("Initialized GLFW window! name: {} width: {} height: {}", mSpecs.name, mSpecs.width,
        mSpecs.height);
}


void FWindowGLFW::terminate() {
  UINFO("Terminating GLFW window! name: {}", mSpecs.name);
  glfwDestroyWindow(mWindowPtr);
  glfwTerminate();
}


void FWindowGLFW::updateFramebufferSize() {
  glfwGetFramebufferSize(mWindowPtr, &mSpecs.width, &mSpecs.height);
}


void FWindowGLFW::pollEvents() {
  updateFramebufferSize();
  glfwPollEvents();
}


void FWindowGLFW::close() {
  UINFO("Closing GLFW window! name: {}", mSpecs.name);
  glfwSetWindowShouldClose(mWindowPtr, GLFW_TRUE);
}


b32 FWindowGLFW::isNotGoingToClose() const {
  return !glfwWindowShouldClose(mWindowPtr);
}


EWindowLibrary FWindowGLFW::getLibrary() const {
  return EWindowLibrary::GLFW;
}


FWindowSpecification FWindowGLFW::getSpecs() const {
  return mSpecs;
}


#ifdef WIN32
HWND FWindowGLFW::getWindowHandle() const {
  return glfwGetWin32Window(mWindowPtr);
}
#endif

}
