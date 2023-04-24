
#include "File.h"
#include "UTools/Logger/Log.h"
#include <stdexcept>
#include <fstream>


namespace uncanny
{


template<typename T>
concept ConceptFileBufferType = std::is_same_v<T, char> or std::is_same_v<T, u32>;


class FFileReader
{
public:

  FFileReader(const char* path, const char* mode)
  {
    errno_t err = fopen_s(&pFile, path, mode);
    if (err != 0)
    {
      UERROR("Cannot open {} file handle!", path);
      throw std::runtime_error("Cannot open file handle");
    }
  }

  ~FFileReader()
  {
    fclose(pFile);
  }

  template<ConceptFileBufferType T>
  void Read(std::vector<T>& buffer)
  {
    // determine file size
    fseek(pFile, 0, SEEK_END);
    long size = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // Resize vector to hold file contents
    buffer.resize(size);

    // Read file into vector
    size_t bytesRead = fread(buffer.data(), 1, size, pFile);
  }

private:

  FILE* pFile{ nullptr };

};


std::vector<char> FFile::Read(const char* pPath)
{
  std::vector<char> outBuffer{};
  FFileReader reader(pPath, "r");
  reader.Read(outBuffer);
  return outBuffer;
}


std::vector<char> FFile::ReadBinary(const char* pPath)
{
  std::vector<char> outBuffer{};
  FFileReader reader(pPath, "rb");
  reader.Read(outBuffer);
  return outBuffer;
}


}
