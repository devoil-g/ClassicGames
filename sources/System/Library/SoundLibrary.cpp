#include <stdexcept>

#include "System/Library/SoundLibrary.hpp"

Game::SoundLibrary::SoundLibrary()
{
  // NOTE: pre-load sounds here
}

void  Game::SoundLibrary::load(const std::filesystem::path& path)
{
  // Element already loaded
  if (_library.find(path) != _library.end())
    return;

  // Load font from path
  if (_library[path].loadFromFile(path.string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}