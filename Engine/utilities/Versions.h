
#ifndef UNCANNYENGINE_VERSIONS_H
#define UNCANNYENGINE_VERSIONS_H


#include "Variables.h"


#define UENGINE_MAKE_VERSION(major, minor, patch) \
    ((((u32)(major)) << 22) | (((u32)(minor)) << 12) | ((u32)(patch)))

#define UENGINE_VERSION_MAJOR(version) ((u32)(version) >> 22)
#define UENGINE_VERSION_MINOR(version) (((u32)(version) >> 12) & 0x3FFU)
#define UENGINE_VERSION_PATCH(version) ((u32)(version) & 0xFFFU)


namespace uncanny
{


enum class EEngineVersion {
  VERSION_0_0_1 = UENGINE_MAKE_VERSION(0, 0, 1),
  LATEST = VERSION_0_0_1
};


u32 displayInfoAboutVersion(u32 version, const char* apiName);


}


#endif //UNCANNYENGINE_VERSIONS_H
