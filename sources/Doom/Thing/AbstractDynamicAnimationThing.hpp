#ifndef _ABSTRACT_DYNAMIC_ANIMATION_THING_HPP_
#define _ABSTRACT_DYNAMIC_ANIMATION_THING_HPP_

#include <array>

#include <SFML/System/Time.hpp>

#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  template<unsigned int FrameDuration = 6>
  class AbstractDynamicAnimationThing : public virtual DOOM::AbstractThing
  {
  private:
    sf::Time																	_elapsed;	// Total elapsed time
    std::vector<std::reference_wrapper<const std::array<std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>, 8>>>	_sprites;	// Vector of sprites according to time/orientation
    
  public:
    AbstractDynamicAnimationThing(DOOM::Doom & doom, const std::string & sprite, const std::string & sequence, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties) :
      DOOM::AbstractThing(doom, thing, height, radius, properties),
      _elapsed(),
      _sprites()
    {
      const std::vector<std::array<std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>, 8>> &	animation = doom.resources.animations.find(DOOM::str_to_key(sprite))->second;

      // Get animations
      for (char frame : sequence)
	_sprites.push_back(std::cref(animation[frame - 'A']));
    }

    virtual ~AbstractDynamicAnimationThing() = default;

    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override	// Update animation sequence
    {
      // Add elapsed time to counter
      _elapsed += elapsed;

      return false;
    }

    virtual const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const override	// Return sprite to be displayed, bool to true if mirrored
    {
      // Return frame according to angle and animation time
      return _sprites[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * FrameDuration) % _sprites.size()].get()[Math::Modulo<8>((int)(angle * 4.f / Math::Pi + 16.5f))];
    }
  };
};

#endif