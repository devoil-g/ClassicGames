#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AnimatedFlat.hpp"	

DOOM::AnimatedFlat::AnimatedFlat(DOOM::Wad const & wad, std::vector<uint64_t> const & frames) :
  AbstractFlat(),
  _elapsed(),
  _flats(frames.size(), std::vector<uint8_t>(sizeof(DOOM::Wad::RawResources::Flat::texture) / sizeof(uint8_t)))
{
  // Convert each frames in animation
  for (int index = 0; index < frames.size(); index++)
  {
    // Find frame in WAD
    std::unordered_map<uint64_t, DOOM::Wad::RawResources::Flat>::const_iterator	rawflat = wad.resources.flats.find(frames[index]);

    // Cancel if error
    if (rawflat == wad.resources.flats.end())
    {
      std::cerr << "[Doom::AnimatedFlat] Warning, flat '" << frames[index] << "' not found in resources." << std::endl;
      continue;
    }

    // Convert WAD flat
    for (int color_index = 0; color_index < sizeof(DOOM::Wad::RawResources::Flat::texture) / sizeof(uint8_t); color_index++)
      _flats[index][color_index] = rawflat->second.texture[color_index];
  }
}

DOOM::AnimatedFlat::~AnimatedFlat()
{}

void	DOOM::AnimatedFlat::update(sf::Time elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;
}

std::vector<uint8_t> const &	DOOM::AnimatedFlat::flat() const
{
  // Return flat of current frame
  return _flats[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::AnimatedFlat::FrameDuration) % _flats.size()];
}