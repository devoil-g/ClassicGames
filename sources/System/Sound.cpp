#include <iostream>
#include <stdexcept>

#include "System/Sound.hpp"

Game::Sound::Reference::Reference(sf::Sound & sound, int & lock) :
  _lock(lock), sound(sound)
{
  // Lock instance
  _lock++;
}

Game::Sound::Reference::~Reference()
{
  // Release instance
  _lock--;

  // Error if no reference is held on a looping sound
  if (_lock <= 0 && sound.getLoop() == true)
    std::cout << "[Game::Sound]: Warning, no reference held on looping sound." << std::endl;
}

Game::Sound::Sound()
  : _sounds()
{}

Game::Sound::~Sound()
{}

bool		Game::Sound::update(sf::Time elapsed)
{
  // Remove unreferenced and not playing sounds
  _sounds.remove_if([](const std::pair<sf::Sound, int>& pair) { return pair.second == 0 && pair.first.getStatus() == sf::Sound::Status::Stopped; });
  
  // Does nothing
  return false;
}

void  Game::Sound::clear()
{
  // Stops every playing sounds
  for (auto& sound : _sounds)
    sound.first.stop();
}

Game::Sound::Reference	Game::Sound::get()
{
  // Check if internal limit has been reached
  if (_sounds.size() + 1 > Game::Sound::MaxSound)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Push a new sound in buffer
  _sounds.emplace_back();

  return Game::Sound::Reference(_sounds.back().first, _sounds.back().second);
}