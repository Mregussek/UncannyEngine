
#ifndef UNCANNYENGINE_IWINDOW_H
#define UNCANNYENGINE_IWINDOW_H


#include "UTools/UTypes.h"
#ifdef WIN32
#include <windows.h>
#else
#error unsupported platform
#endif


namespace uncanny
{


struct FWindowSize
{
  i32 width{ 0 };
  i32 height{ 0 };
};


struct FMousePosition
{
  f64 x{ 0.f };
  f64 y{ 0.f };
};


struct FMouseButtonsPressed
{
  b32 left{ UFALSE };
  b32 right{ UFALSE };
};


struct FMouseScrollPosition
{
  f64 x{ 0.f };
  f64 y{ 0.f };
};


struct FKeyboardButtonsPressed
{
  b32 escape{ UFALSE };
  b32 w{ UFALSE };
  b32 s{ UFALSE };
  b32 a{ UFALSE };
  b32 d{ UFALSE };
  b32 num1{ UFALSE };
  b32 num2{ UFALSE };
  b32 leftShift{ UFALSE };
  b32 leftControl{ UFALSE };
  b32 space{ UFALSE };
};


struct FWindowConfiguration
{
  u32 resizable{ UFALSE };
  b32 fullscreen{ UFALSE };
  FWindowSize size{};
  const char* name{ "" };
};


class IWindow
{
public:

  virtual void Create(const FWindowConfiguration& windowConfiguration) = 0;
  virtual void Destroy() = 0;

  virtual void UpdateState() = 0;
  virtual void PollEvents() const = 0;

  virtual void Close() = 0;

  [[nodiscard]] virtual FWindowSize GetSize() const = 0;
  [[nodiscard]] virtual FMousePosition GetMousePosition() const = 0;
  [[nodiscard]] virtual FMouseButtonsPressed GetMouseButtonsPressed() const = 0;
  [[nodiscard]] virtual FMouseScrollPosition GetMouseScrollPosition() const = 0;
  [[nodiscard]] virtual const FKeyboardButtonsPressed& GetKeyboardButtonsPressed() const = 0;

  [[nodiscard]] virtual b32 IsGoingToClose() const = 0;

  [[nodiscard]] virtual b32 IsMinimized() const = 0;

  [[nodiscard]] virtual f32 GetDeltaTime() const = 0;

#ifdef WIN32
  [[nodiscard]] virtual HWND GetWin32Handle() const = 0;
#endif

};


}


#endif //UNCANNYENGINE_IWINDOW_H
