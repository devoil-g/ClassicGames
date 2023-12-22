#include <fstream>
#include <stdexcept>

#include "System/Library/FileHashLibrary.hpp"

Game::FileHashLibrary::FileHashLibrary()
{
  // NOTE: pre-load file hashes here
}

void  Game::FileHashLibrary::load(const std::filesystem::path& path)
{
  // Element already loaded
  if (_library.find(path) != _library.end())
    return;

  std::ifstream file(path, std::iostream::binary);

  // Failed to open file
  if (!file)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  std::string   data;
  std::uint8_t  byte;

  // Read file to string
  data.reserve(1024 * 1024);
  while (file >> byte)
    data.push_back(byte);

  // Hash file
  _library[path] = std::size_t(std::hash<std::string>{}(data));
}