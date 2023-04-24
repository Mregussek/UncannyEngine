
#ifndef UNCANNYENGINE_FILE_H
#define UNCANNYENGINE_FILE_H


#include <vector>
#include "UTools/UTypes.h"


namespace uncanny
{


class FFile
{
public:

  static std::vector<char> Read(const char* pPath);

  static std::vector<char> ReadBinary(const char* pPath);

};


}


#endif //UNCANNYENGINE_FILE_H
