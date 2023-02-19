
#include <iostream>
#include <UTools/Logger/Log.h>
#include <UGraphicsEngine/RHI/RHI.h>

using namespace uncanny;

int main() {
  FLogger::init(FLogger::sLoggerPtr);

  RHI rhi;

  UTRACE("Hello Trace!");

  return 0;
}
