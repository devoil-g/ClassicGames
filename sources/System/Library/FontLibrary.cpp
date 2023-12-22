#include <stdexcept>

#include "System/Config.hpp"
#include "System/Library/FontLibrary.hpp"

Game::FontLibrary::FontLibrary()
{
  // NOTE: pre-load fonts here
  load(Game::Config::ExecutablePath / "assets" / "fonts" / "pixelated.ttf");
}

void  Game::FontLibrary::load(const std::filesystem::path& path)
{
  // Element already loaded
  if (_library.find(path) != _library.end())
    return;

  // Load font from path
  if (_library[path].loadFromFile(path.string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}