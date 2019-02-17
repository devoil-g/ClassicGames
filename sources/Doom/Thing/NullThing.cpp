#include "Doom/Thing/NullThing.hpp"

DOOM::NullThing::NullThing(DOOM::Doom & doom, DOOM::Wad::RawLevel::Thing const & thing) :
  DOOM::AbstractThing(doom, thing, 0, 0, None),
  DOOM::AbstractNonePhysicsThing(doom, thing, 0, 0, None),
  DOOM::AbstractNoneAnimationThing(doom, thing, 0, 0, None),
  DOOM::AbstractNonePickupThing(doom, thing, 0, 0, None)
{}

bool	DOOM::NullThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Does nothing
  return false;
}