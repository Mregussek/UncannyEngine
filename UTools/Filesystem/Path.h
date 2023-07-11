
#ifndef UNCANNYENGINE_PATH_H
#define UNCANNYENGINE_PATH_H


#include <string>
#include <filesystem>
#include "UTools/UTypes.h"


namespace uncanny
{


class FPath
{
public:

  explicit FPath(const char* pPath);
  explicit FPath(std::string path);

  /// @returns path to .exe file on win32 OS
  /// "C:\\Users\\mateu\\Projects\\UncannyEngine\\clion-build\\USamples\\00_Sandbox\\Debug\\00_Sandbox.exe"
  [[nodiscard]] static FPath GetExecutableFilePath();

  /// @returns path to .exe file on win32 OS
  /// "C:\\Users\\mateu\\Projects\\UncannyEngine\\clion-build\\USamples\\00_Sandbox\\Debug"
  [[nodiscard]] static FPath GetExecutablePath();

  /// @brief Method gets and returns path to UncannyEngine project. Retrieves it from executable path.
  /// @returns path to UncannyEngine.
  [[nodiscard]] static FPath GetEngineProjectPath();

  [[nodiscard]] static std::vector<FPath> GetFilePathsInDirectory(const FPath& directory);

  /// @brief For given path discards all path section till the given directory name.
  /// @details For example for given path:
  /// "C:\\Users\\mateu\\Projects\\UncannyEngine\\clion-build\\USamples\\00_Sandbox\\Debug\\00_Sandbox.exe"
  /// and for given directory name: "UncannyEngine"
  /// returned value will be: "C:\\Users\\mateu\\Projects\\UncannyEngine"
  /// @returns path till the given directory name
  [[nodiscard]] static FPath DiscardPathTillDirectory(const FPath& path, const char* directoryName);

  [[nodiscard]] static FPath Append(const FPath& path, const char* pPathElement);

  [[nodiscard]] static FPath Append(const FPath& path, std::initializer_list<const char*> pathElements);

  [[nodiscard]] static b32 HasExtension(const FPath& path, const char* ext);

  [[nodiscard]] static b32 Exists(const FPath& path);

  static b32 Delete(const FPath& path);

  [[nodiscard]] static std::string GetFilename(const FPath& path);

  [[nodiscard]] const std::string& GetStringPath() const { return m_Path; }

  [[nodiscard]] const std::string& GetStringFilename() const { return m_Filename; }

private:

  std::string m_Path{};
  std::string m_Filename{};

};


}


#endif //UNCANNYENGINE_PATH_H
