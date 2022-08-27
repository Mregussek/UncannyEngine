
#include "FileManager.h"
#include <utilities/Includes.h>
#include <utilities/Logger.h>
#if WIN32
  #include <windows.h>
#else
#error "Unknown OS for FFileManager.cpp"
#endif


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


b32 FFileManager::getExecutablePath(char* pOutPath, u32 sizeofBuffer) {
#ifdef WIN32
  // When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
  HMODULE hModule = GetModuleHandle(nullptr);
  if (hModule != nullptr) {
    // Use GetModuleFileName() with module handle to get the path
    GetModuleFileName(hModule, pOutPath, sizeofBuffer);
    UTRACE("Retrieved {} executable path!", pOutPath);
    return UTRUE;
  }
  else {
    UERROR("Module handle is NULL, cannot retrieve executable path!");
    return UFALSE;
  }
#else
  UERROR("Unknown OS, cannot retrieve executable path!");
  return UFALSE;
#endif // WIN32
}


}
