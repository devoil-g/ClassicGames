#include "System/Library/FontLibrary.hpp"
#include "System/Config.hpp"

Game::FontLibrary::FontLibrary()
{
  // Pre-load fonts here
  load(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf");
}

Game::FontLibrary::~FontLibrary()
{}

void	Game::FontLibrary::load(std::string const & path)
{
  // Load font from path
  if (_library[path].loadFromFile(path) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}
