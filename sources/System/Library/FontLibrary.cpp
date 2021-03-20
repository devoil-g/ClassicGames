#include "System/Library/FontLibrary.hpp"
#include "System/Config.hpp"

Game::FontLibrary::FontLibrary()
{
  // Pre-load fonts here
  load(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf");
}

void  Game::FontLibrary::load(const std::string& path)
{
  // Load font from path
  if (_library[path].loadFromFile(path) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}
