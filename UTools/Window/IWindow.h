
#ifndef UNCANNYENGINE_IWINDOW_H
#define UNCANNYENGINE_IWINDOW_H


#include "UTools/UTypes.h"


namespace uncanny {


struct FWindowConfiguration {
  u32 resizable{ UFALSE };
  b32 fullscreen{ UFALSE };
  i32 width{ 0 };
  i32 height{ 0 };
  const char* name{ "" };
};


class IWindow {
public:

  virtual ~IWindow() = default;
  
  virtual void Create(const FWindowConfiguration& windowConfiguration) = 0;
  virtual void Destroy() = 0;

  virtual void UpdateSize() = 0;
  virtual void PollEvents() const = 0;

  virtual void Close() = 0;

  [[nodiscard]] virtual b32 IsGoingToClose() const = 0;

  [[nodiscard]] virtual b32 IsMinimized() const = 0;

#ifdef WIN32
  [[nodiscard]] virtual HWND GetWin32Handle() const = 0;
#endif

};



}


#endif //UNCANNYENGINE_IWINDOW_H
