
#include "RenderContextDependencies.h"
#include <utilities/Logger.h>


namespace uncanny
{


FQueueFamilyDependencies getQueueFamilyDependencies(
    EQueueFamilyMainUsage mainUsage,
    const std::vector<FQueueFamilyDependencies>& queueFamilyDependenciesVector) {
  auto it = std::find_if(queueFamilyDependenciesVector.begin(), queueFamilyDependenciesVector.end(),
                         [mainUsage](const FQueueFamilyDependencies& dependencies) -> b32 {
                           return dependencies.mainUsage == mainUsage;
                         });
  if (it != queueFamilyDependenciesVector.end()) {
    UTRACE("Found queue family dependencies with main usage {}", (i32)mainUsage);
    return *it;
  }

  UTRACE("Could not find queue family dependencies with main usage {}", (i32)mainUsage);
  return {};
}


}
