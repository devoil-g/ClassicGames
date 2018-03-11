#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AnimatedFlat.hpp"	

Game::AnimatedFlat::AnimatedFlat(Game::Wad const & wad, std::vector<uint64_t> const & frames) :
  AbstractFlat(),
  _elapsed(),
  _flats(frames.size(), std::vector<uint8_t>(sizeof(Game::Wad::RawResources::Flat::texture) / sizeof(uint8_t)))
{
  // Convert each frames in animation
  for (int index = 0; index < frames.size(); index++)
  {
    // Find frame in WAD
    std::unordered_map<uint64_t, Game::Wad::RawResources::Flat>::const_iterator	rawflat = wad.resources.flats.find(frames[index]);

    // Cancel if error
    if (rawflat == wad.resources.flats.end())
    {
      std::cerr << "[Doom::AnimatedFlat] Warning, flat '" << frames[index] << "' not found in resources." << std::endl;
      continue;
    }

    // Convert WAD flat
    for (int color_index = 0; color_index < sizeof(Game::Wad::RawResources::Flat::texture) / sizeof(uint8_t); color_index++)
      _flats[index][color_index] = rawflat->second.texture[color_index];
  }
}

Game::AnimatedFlat::~AnimatedFlat()
{}

void	Game::AnimatedFlat::update(sf::Time elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;
}

std::vector<uint8_t> const &	Game::AnimatedFlat::flat() const
{
  // Return flat of current frame
  return _flats[_elapsed.asMicroseconds() / (Game::Doom::Tic.asMicroseconds() * Game::AnimatedFlat::FrameDuration) % _flats.size()];
}