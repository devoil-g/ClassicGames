#pragma once

#include <list>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Game
{
  class Sound
  {
  public:
    class Reference
    {
      friend class Game::Sound;

    private:
      int&  _lock;  // Reference to lock of sound

      Reference(sf::Sound& sound, int& lock);
      Reference(const Reference& ref);

      Reference(Reference&& ref) = delete;
      Reference& operator=(const Reference& ref) = delete;
      Reference& operator=(Reference&& ref) = delete;

    public:
      sf::Sound&  sound;  // Reference to sound to be played

      ~Reference();
    };

    static const int  MaxSound = 256; // Maximum number of soundss

    std::list<std::pair<sf::Sound, int>>  _sounds;  // Sound instances

    Sound() = default;
    ~Sound() = default;

  public:    
    inline static Game::Sound&  Instance() { static Game::Sound singleton; return singleton; }; // Get instance (singleton)

    bool                    update(sf::Time); // Update sounds
    void                    clear();          // Interrupt every sound currently playing
    Game::Sound::Reference  get();            // Get an available sound
  };
}