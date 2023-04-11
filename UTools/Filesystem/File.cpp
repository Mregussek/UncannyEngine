
#include "File.h"
#include "UTools/Logger/Log.h"
#include <stdexcept>


namespace uncanny
{


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

  void Read(std::vector<char>& buffer)
  {
    // determine file size
    fseek(pFile, 0, SEEK_END);
    long size = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // Resize vector to hold file contents
    buffer.resize(size);

    // Read file into vector
    size_t bytesRead = fread(buffer.data(), 1, size, pFile);
    if (bytesRead != size)
    {
      UERROR("Read error during loading!");
      throw std::runtime_error("Read error during loading!");
    }
  }

private:

  FILE* pFile{ nullptr };

};


std::vector<char> FFile::Read(const char* pPath)
{
  std::vector<char> outBuffer{};
  FFileReader reader(pPath, "rb");
  reader.Read(outBuffer);
  return outBuffer;
}


}
