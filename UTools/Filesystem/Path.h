
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
  static FPath GetExecutableFilePath();

  /// @returns path to .exe file on win32 OS
  /// "C:\\Users\\mateu\\Projects\\UncannyEngine\\clion-build\\USamples\\00_Sandbox\\Debug"
  static FPath GetExecutablePath();

  /// @brief Method gets and returns path to UncannyEngine project. Retrieves it from executable path.
  /// @returns path to UncannyEngine.
  static FPath GetEngineProjectPath();

  /// @brief For given path discards all path section till the given directory name.
  /// @details For example for given path:
  /// "C:\\Users\\mateu\\Projects\\UncannyEngine\\clion-build\\USamples\\00_Sandbox\\Debug\\00_Sandbox.exe"
  /// returned value will be: "C:\\Users\\mateu\\Projects\\UncannyEngine"
  /// @returns path till the given directory name
  static FPath DiscardPathTillDirectory(const FPath& path, const char* directoryName);

  static FPath Append(const FPath& path, const char* pPathElement);

  static FPath Append(const FPath& path, std::initializer_list<const char*> pathElements);

  static b32 HasExtension(const FPath& path, const char* ext);

  static b32 Exists(const FPath& path);

  static b32 Delete(const FPath& path);

  [[nodiscard]] const std::string& GetString() const { return m_Path; }

private:

  std::string m_Path{};

};


}


#endif //UNCANNYENGINE_PATH_H
