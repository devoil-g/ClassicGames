#include <stdexcept>

#include "System/Library/SoundLibrary.hpp"

Game::SoundLibrary::SoundLibrary()
{
  // NOTE: pre-load sounds here
}

void  Game::SoundLibrary::load(const std::string& path)
{
  // Load font from path
  if (_library[path].loadFromFile(path) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}