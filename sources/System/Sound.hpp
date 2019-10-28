#ifndef _SOUND_HPP_
#define _SOUND_HPP_

#include <vector>

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
      int &	_lock; // Reference to lock of sound

      Reference(sf::Sound & sound, int & lock);

    public:
      sf::Sound &	sound;	// Reference to sound to be played

      ~Reference();
    };

    static const int	MaxSound = 256;	// Maximum number of soundss
    
    std::vector<std::pair<sf::Sound, int>>	_sounds;	// Sound instances

  public:
    Sound();
    ~Sound();

    inline static Game::Sound &	Instance() { static Game::Sound singleton; return singleton; };	// Get instance (singleton)

    bool			update(sf::Time);	// Update sounds
    Game::Sound::Reference	get();			// Get an available sound
  };
};

#endif