#include <stdexcept>

#include "System/Library/TextureLibrary.hpp"
#include "System/Config.hpp"

Game::TextureLibrary::TextureLibrary()
{
  // Pre-load textures here
}

void  Game::TextureLibrary::load(const std::string& path)
{
  // Load texture from path
  if (_library[path].loadFromFile(path) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}
