#ifndef _ABSTRACT_NONE_ANIMATION_THING_HPP_
#define _ABSTRACT_NONE_ANIMATION_THING_HPP_

#include <array>

#include <SFML/System/Time.hpp>

#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class AbstractNoneAnimationThing : public virtual DOOM::AbstractThing
  {
  public:
    AbstractNoneAnimationThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties);
    virtual ~AbstractNoneAnimationThing() = default;

    virtual bool											update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update animation sequence
    virtual const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const override;			// Return sprite to be displayed, bool to true if mirrored
  };
};

#endif