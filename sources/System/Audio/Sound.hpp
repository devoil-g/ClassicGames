#pragma once

#include <list>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Game
{
  namespace Audio
  {
    class Sound
    {
    public:
      class Reference
      {
        friend class Game::Audio::Sound;

      private:
        int& _lock;  // Reference to lock of sound

        Reference(sf::Sound& sound, sf::SoundBuffer& buffer, int& lock);
        Reference(const Reference& ref);

        Reference(Reference&& ref) = delete;
        Reference& operator=(const Reference& ref) = delete;
        Reference& operator=(Reference&& ref) = delete;

      public:
        sf::SoundBuffer&  buffer; // Optional sound buffer
        sf::Sound&        sound;  // Reference to sound to be played

        ~Reference();
      };

      static const int  MaxSound = 256; // Maximum number of soundss

      struct Element
      {
        sf::SoundBuffer buffer;
        sf::Sound       sound;
        int             lock;

        Element();
        Element(const Element&) = delete;
        Element(Element&&) = delete;
        ~Element() = default;
      };

      std::list<Element>  _sounds;  // Sound instances

      Sound() = default;
      ~Sound() = default;

    public:
      inline static Game::Audio::Sound& Instance() { static Game::Audio::Sound singleton; return singleton; };  // Get instance (singleton)

      bool                          update(float elapsed);  // Update sounds
      void                          clear();                // Interrupt every sound currently playing
      Game::Audio::Sound::Reference get();                  // Get an available sound
    };
  }
}