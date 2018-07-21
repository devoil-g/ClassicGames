#ifndef _ANIMATED_THING_HPP_
#define _ANIMATED_THING_HPP_

#include <array>

#include <SFML/System/Time.hpp>

#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class Doom;

  template<unsigned int FrameDuration = 8>
  class AnimatedThing : public virtual DOOM::AbstractThing
  {
  private:
    sf::Time													_elapsed;	// Total elapsed time
    std::vector<std::array<std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>, 8>>	_sprites;	// Vector of sprites according to time/orientation
    
  public:
    AnimatedThing(DOOM::Doom & doom, const std::string & sprite, const std::string & sequence, const DOOM::Wad::RawLevel::Thing & thing, int16_t radius, int16_t properties) :
      DOOM::AbstractThing(doom, thing, radius, properties),
      _elapsed(),
      _sprites(sequence.length(), { {
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	{ std::ref(DOOM::Doom::Resources::Texture::Null), false },
	} })
    {
      // Find texture in DOOM resources associated with sprite and sequence
      for (const std::pair<const uint64_t, DOOM::Doom::Resources::Texture> & texture : doom.resources.sprites)
	if ((texture.first & 0x00000000FFFFFFFF) == DOOM::str_to_key(sprite))
	  for (int index = 0; index < sequence.length(); index++)
	  {
	    // Match texture
	    if (((texture.first & 0x000000FF00000000) >> 32) == sequence[index] &&
	      ((texture.first & 0x0000FF0000000000) >> 40) >= '0' && ((texture.first & 0x0000FF0000000000) >> 40) <= '8')
	    {
	      if (((texture.first & 0x0000FF0000000000) >> 40) == '0')
		for (int angle = 0; angle < 8; angle++)
		  _sprites[index][angle] = { std::cref(texture.second), false };
	      else
		_sprites[index][((texture.first & 0x0000FF0000000000) >> 40) - '0'] = { std::cref(texture.second), false };
	    }

	    // Match mirrored texture
	    if (((texture.first & 0x00FF000000000000) >> 48) == sequence[index] &&
	      ((texture.first & 0xFF00000000000000) >> 56) >= '0' && ((texture.first & 0xFF00000000000000) >> 56) <= '8')
	    {
	      if (((texture.first & 0xFF00000000000000) >> 56) == '0')
		for (int angle = 0; angle < 8; angle++)
		  _sprites[index][angle] = { std::cref(texture.second), true };
	      else
		_sprites[index][((texture.first & 0xFF00000000000000) >> 56) - '0'] = { std::cref(texture.second), true };
	    }
	  }
    }

    virtual ~AnimatedThing() = default;

    virtual bool								update(DOOM::Doom & doom, sf::Time elapsed) override	// Update animation sequence
    {
      // Add elapsed time to counter
      _elapsed += elapsed;

      return false;
    }

    virtual const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float) const override	// Return sprite to be displayed, bool to true if mirrored
    {
      // TODO: find angle
      return _sprites[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * FrameDuration) % _sprites.size()][0];
    }
  };
};

#endif