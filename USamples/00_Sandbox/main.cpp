
#include <UTools/Logger/Log.h>
#include <UGraphicsEngine/RHI/RHI.h>

using namespace uncanny;

int main() {
  // checking tools include
  FLogger::init(FLogger::sLoggerPtr);
  UINFO("Hello UncannyEngine!");

  // checking graphics engine include
  RHI rhi;

  return 0;
}
