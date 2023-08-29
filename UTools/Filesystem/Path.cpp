
#include "Path.h"
#include "UTools/Logger/Log.h"
#include <filesystem>
#include <utility>
#if WIN32
  #define U_WIN32_USED 1
  #include <windows.h>
#else
  #error no supported platform
#endif


namespace uncanny
{


FPath::FPath(const char* pPath)
  : m_Path(pPath), m_Filename(GetFilename(*this))
{
}


FPath::FPath(std::string path)
  : m_Path(std::move(path)), m_Filename(GetFilename(*this))
{
}


FPath FPath::GetExecutableFilePath()
{
  if constexpr (U_WIN32_USED)
  {
    char ownPath[MAX_PATH];

    // When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
    HMODULE hModule = GetModuleHandle(nullptr);
    if (not hModule)
    {
      const char* errorCode = "Could not find hModule for WIN32 API during executable path retrieval!";
      UERROR(errorCode);
      throw std::runtime_error(errorCode);
    }

    // Use GetModuleFileName() with module handle to get the path
    GetModuleFileName(hModule, ownPath, sizeof(ownPath));
    return FPath{ ownPath };
  }
  else
  {
    std::string errorCode = "No supported platform!";
    UCRITICAL(errorCode.c_str());
    throw std::runtime_error(errorCode);
  }
}


FPath FPath::GetExecutablePath()
{
  std::filesystem::path filepath{ GetExecutableFilePath().GetStringPath() };
  return FPath{ filepath.parent_path().string() };
}


FPath FPath::GetEngineProjectPath()
{
  const char* uncannyEngine = "UncannyEngine";
  FPath path = DiscardPathTillDirectory(GetExecutableFilePath(), uncannyEngine);
  return Append(path, uncannyEngine);
}


std::vector<FPath> FPath::GetFilePathsInDirectory(const FPath& directory)
{
  std::vector<FPath> rtn{};
  for (const auto& entry : std::filesystem::directory_iterator(directory.GetStringPath()))
  {
    rtn.emplace_back(entry.path().string());
  }
  return rtn;
}


FPath FPath::DiscardPathTillDirectory(const FPath& path, const char* directoryName)
{
  size_t foundIdx = path.m_Path.find(directoryName);
  if (foundIdx == std::string::npos)
  {
    UERROR("No {} in path found!", directoryName);
    throw std::runtime_error("No path found during discarding till directory!");
  }
  return FPath{ path.m_Path.substr(0, foundIdx) };
}


FPath FPath::Append(const FPath& path, const char* pPathElement)
{
  std::filesystem::path absolute{ path.m_Path };
  std::filesystem::path elem{ pPathElement };
  absolute /= elem;
  return FPath{ absolute.string() };
}


FPath FPath::Append(const FPath& path, std::initializer_list<const char*> pathElements)
{
  FPath rtn{ path };
  std::ranges::for_each(pathElements, [&rtn](const char* pElem)
  {
    rtn = Append(rtn, pElem);
  });
  return rtn;
}


b32 FPath::HasExtension(const FPath& path, const char* ext)
{
  std::filesystem::path absolute{ path.m_Path };
  return absolute.extension() == std::string{ ext };
}


b32 FPath::Exists(const FPath& path)
{
  std::filesystem::path filepath{ path.GetStringPath() };
  return std::filesystem::exists(filepath);
}


b32 FPath::Delete(const FPath &path)
{
  if (not Exists(path))
  {
    UERROR("Calling Delete() function on path {} that does not exist!", path.GetStringPath());
    return UFALSE;
  }

  std::filesystem::path filepath{ path.GetStringPath() };
  return std::filesystem::remove(filepath);
}


std::string FPath::GetFilename(const FPath& path)
{
  std::filesystem::path filepath{ path.GetStringPath() };
  return filepath.filename().string();
}


}
