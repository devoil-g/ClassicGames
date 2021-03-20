#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>

#include "System/Library/AbstractLibrary.hpp"

namespace Game
{
  class FontLibrary : public Game::AbstractLibrary<std::string, sf::Font>
  {
  private:
    FontLibrary();
    ~FontLibrary() override = default;

  public:
    inline static Game::FontLibrary&  Instance() { static Game::FontLibrary singleton; return singleton; }; // Return unique instance (singleton)

    void  load(const std::string&) override;  // Load a font in the library
  };
}