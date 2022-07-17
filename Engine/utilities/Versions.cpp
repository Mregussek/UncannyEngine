
#include "Versions.h"
#include "Variables.h"
#include "Logger.h"


namespace uncanny
{


u32 displayInfoAboutVersion(u32 version, const char* apiName) {
  const u32 major{ UENGINE_VERSION_MAJOR(version) };
  const u32 minor{ UENGINE_VERSION_MINOR(version) };
  const u32 patch{ UENGINE_VERSION_PATCH(version) };
  UINFO("Chosen {} version: Major {} Minor {} Patch {}", apiName, major, minor, patch);
  return UENGINE_MAKE_VERSION(major, minor, patch);
}


}
