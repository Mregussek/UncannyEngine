
#ifndef UNCANNYENGINE_WINDOW_H
#define UNCANNYENGINE_WINDOW_H


#include <utilities/Includes.h>


namespace uncanny
{


struct FWindowSpecification {
  const char* name{ "" };
  i32 width{ 0 };
  i32 height{ 0 };
};


class FWindow {
public:

  virtual void init(FWindowSpecification windowSpecification);
  virtual void terminate();

  virtual void swapBuffersAndPollEvents();

  virtual void close();
  [[nodiscard]] virtual b32 isNotGoingToClose() const;

  [[nodiscard]] virtual EWindowLibrary getLibrary() const;
  [[nodiscard]] virtual FWindowSpecification getSpecs() const;

};


}


#endif //UNCANNYENGINE_WINDOW_H
