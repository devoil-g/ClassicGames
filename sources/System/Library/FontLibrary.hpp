#pragma once

#include <filesystem>

#include <SFML/Graphics/Font.hpp>

#include "System/Library/AbstractLibrary.hpp"

namespace Game
{
  class FontLibrary : public Game::AbstractLibrary<std::filesystem::path, sf::Font>
  {
  private:
    FontLibrary();
    ~FontLibrary() override = default;

  public:
    inline static Game::FontLibrary&  Instance() { static Game::FontLibrary singleton; return singleton; }; // Return unique instance (singleton)

    void  load(const std::filesystem::path&) override;  // Load a font in the library
  };
}