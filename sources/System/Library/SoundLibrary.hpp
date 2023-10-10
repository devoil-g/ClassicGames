#pragma once

#include <string>

#include <SFML/Audio/SoundBuffer.hpp>

#include "System/Library/AbstractLibrary.hpp"

namespace Game
{
  class SoundLibrary : public Game::AbstractLibrary<std::string, sf::SoundBuffer>
  {
  private:
    SoundLibrary();
    ~SoundLibrary() override = default;

  public:
    inline static Game::SoundLibrary&  Instance() { static Game::SoundLibrary singleton; return singleton; }; // Return unique instance (singleton)

    void  load(const std::string&) override;  // Load a font in the library
  };
}