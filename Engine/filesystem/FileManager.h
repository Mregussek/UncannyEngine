
#ifndef UNCANNYENGINE_FILESYSTEM_H
#define UNCANNYENGINE_FILESYSTEM_H


#include <utilities/Variables.h>
#include <vector>


namespace uncanny
{


class FFileManager {
public:

  static b32 loadFile(const char* path, std::vector<char>* pOutBuffer);

  static b32 getExecutablePath(char* pOutBuffer, u32 sizeofBuffer);

};


}


#endif //UNCANNYENGINE_FILESYSTEM_H
