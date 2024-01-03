
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

  FPath() = default;
  explicit FPath(const char* pPath);
  explicit FPath(std::string path);

public:

  /// @brief Adds path element to given path.
  /// @return newly created path with added path element
  [[nodiscard]] static FPath Append(const FPath& path, const char* pPathElement);

  /// @brief Adds several path elements to given path.
  /// @return newly created path with added path elements
  [[nodiscard]] static FPath Append(const FPath& path, std::initializer_list<const char*> pathElements);

  /// @brief Check if given path has given extension.
  /// @return success status
  [[nodiscard]] static b32 HasExtension(const FPath& path, const char* ext);

  /// @brief Checks if given path exists in filesystem.
  /// @return success status
  [[nodiscard]] static b32 Exists(const FPath& path);

  /// @brief Deletes path from filesystem.
  /// @return success status
  static b32 Delete(const FPath& path);

  /// @brief For given path discards all path section till the given directory name.
  /// @details For example for given path:
  /// "C:\\Projects\\UncannyEngine\\build\\USamples\\00_Sandbox\\Debug\\00_Sandbox.exe"
  /// and for given directoryName: "UncannyEngine"
  /// returned value will be: "C:\\Projects\\UncannyEngine"
  /// @returns path till the given directory name
  [[nodiscard]] static FPath DiscardPathTillDirectory(const FPath& path, const char* directoryName);

// Getters
public:

  /// @returns file path to .exe file
  /// "C:\\Projects\\UncannyEngine\\build\\USamples\\00_Sandbox\\Debug\\00_Sandbox.exe"
  [[nodiscard]] static FPath GetExecutableFilePath();

  /// @returns path to .exe file
  /// "C:\\Projects\\UncannyEngine\\build\\USamples\\00_Sandbox\\Debug"
  [[nodiscard]] static FPath GetExecutablePath();

  /// @brief Method gets and returns path to UncannyEngine project. Retrieves it from executable path.
  /// @returns path to UncannyEngine, ex. "C:\\Projects\\UncannyEngine"
  [[nodiscard]] static FPath GetEngineProjectPath();

  /// @brief Iterates through all files in given directory and returns file paths.
  [[nodiscard]] static std::vector<FPath> GetFilePathsInDirectory(const FPath& directory);

  /// @brief For given path returns filename with extension.
  [[nodiscard]] static std::string GetFilename(const FPath& path);

  [[nodiscard]] const std::string& GetStringPath() const { return m_Path; }

  [[nodiscard]] const std::string& GetStringFilename() const { return m_Filename; }

private:

  std::string m_Path{};
  std::string m_Filename{};

};


}


#endif //UNCANNYENGINE_PATH_H
