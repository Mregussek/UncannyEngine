
#include "Window.h"
#include <utilities/Logger.h>


namespace uncanny
{


void FWindow::init(FWindowSpecification windowSpecification) {
  UFATAL("should not be used!");
}


void FWindow::terminate() {
  UFATAL("should not be used!");
}


void FWindow::updateFramebufferSize() {
  UFATAL("should not be used");
}


void FWindow::pollEvents() {
  UFATAL("should not be used!");
}


void FWindow::close() {
  UFATAL("should not be used!");
}


b32 FWindow::isNotGoingToClose() const {
  UFATAL("should not be used!");
  return UFALSE;
}


b32 FWindow::isMinimized() const {
  UFATAL("should not be used!");
  return UFALSE;
}


EWindowLibrary FWindow::getLibrary() const {
  UFATAL("should not be used!");
  return EWindowLibrary::NONE;
}


FWindowSpecification FWindow::getSpecs() const {
  UFATAL("should not be used!");
  return {};
}


}
