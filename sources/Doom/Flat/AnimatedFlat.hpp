#ifndef _ANIMATED_FLAT_HPP_
#define _ANIMATED_FLAT_HPP_

#include <unordered_map>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  template<unsigned int FrameDuration = 8>
  class AnimatedFlat : public virtual DOOM::Doom::Resources::AbstractFlat
  {
  private:
    sf::Time					_elapsed;	// Total elapsed time
    std::vector<std::array<uint8_t, 4096>>	_flats;		// Vector of flats composing animation
    
  public:
    AnimatedFlat(DOOM::Doom & doom, const std::vector<uint64_t> & frames) :
      DOOM::Doom::Resources::AbstractFlat(doom),
      _elapsed(),
      _flats(frames.size())
    {
      // Convert each frames in animation
      for (int index = 0; index < frames.size(); index++)
      {
	// Find frame in WAD
	std::unordered_map<uint64_t, DOOM::Wad::RawResources::Flat>::const_iterator	rawflat = doom.wad.resources.flats.find(frames[index]);

	// Cancel if error
	if (rawflat == doom.wad.resources.flats.end())
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

	// Convert WAD flat
	std::memcpy(_flats[index].data(), rawflat->second.texture, sizeof(DOOM::Wad::RawResources::Flat::texture));
      }
    }

    ~AnimatedFlat() override = default;

    void				update(DOOM::Doom & doom, sf::Time elapsed) override	// Update animation sequence
    {
      // Add elapsed time to counter
      _elapsed += elapsed;
    }

    const std::array<uint8_t, 4096> &	flat() const override	// Return flat to be displayed
    {
      // Return flat of current frame
      return _flats[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * FrameDuration) % _flats.size()];
    }
  };
};

#endif