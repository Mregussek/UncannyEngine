
#include "FileManager.h"
#include <utilities/Includes.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FFileManager::loadFile(const char* path, std::vector<char>* pOutBuffer) {
  UTRACE("Loading file {} ...", path);

  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    UERROR("Could not find {} filepath, check if it exists!", path);
    return UFALSE;
  }

  i64 fileSize{ file.tellg() };
  pOutBuffer->resize(fileSize);

  file.seekg(0);
  file.read(pOutBuffer->data(), fileSize);
  file.close();

  UDEBUG("Loaded file {}!", path);
  return UTRUE;
}


}
