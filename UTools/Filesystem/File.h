
#ifndef UNCANNYENGINE_FILE_H
#define UNCANNYENGINE_FILE_H


#include <vector>


namespace uncanny
{


class FFile
{
public:

  static std::vector<char> Read(const char* pPath);

};


}


#endif //UNCANNYENGINE_FILE_H
