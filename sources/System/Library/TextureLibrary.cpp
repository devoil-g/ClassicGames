#include <stdexcept>

#include "System/Library/TextureLibrary.hpp"

Game::TextureLibrary::TextureLibrary()
{
  // NOTE: pre-load textures here
}

void  Game::TextureLibrary::load(const std::filesystem::path& path)
{
  // Element already loaded
  if (_library.find(path) != _library.end())
    return;

  // Load texture from path
  if (_library[path].loadFromFile(path.string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}