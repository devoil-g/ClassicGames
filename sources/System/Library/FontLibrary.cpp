#include <filesystem>
#include <stdexcept>

#include "System/Config.hpp"
#include "System/Library/FontLibrary.hpp"

Game::FontLibrary::FontLibrary()
{
  // Preload fonts
  for (const auto& path : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "fonts"))
  {
    // Attempt to load file as a font
    try {
      load(path);
    }

    // Ignore failures
    catch (...) {
      continue;
    }
  }
}

void  Game::FontLibrary::load(const std::filesystem::path& path)
{
  // Element already loaded
  if (_library.find(path) != _library.end())
    return;

  // Load font from path
  if (_library[path].openFromFile(path.string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}