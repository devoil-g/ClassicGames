#pragma once

#include <filesystem>

#include <SFML/Graphics/Texture.hpp>

#include "System/Library/AbstractLibrary.hpp"

namespace Game
{
  class TextureLibrary : public Game::AbstractLibrary<std::filesystem::path, sf::Texture>
  {
  private:
    TextureLibrary();
    ~TextureLibrary() override = default;

  public:
    inline static Game::TextureLibrary& Instance() { static Game::TextureLibrary singleton; return singleton; };  // Return unique instance (singleton)

    void  load(const std::filesystem::path&) override;  // Load a texture in the library
  };
}