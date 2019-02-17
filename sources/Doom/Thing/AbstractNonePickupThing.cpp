#include "Doom/Thing/AbstractNonePickupThing.hpp"

DOOM::AbstractNonePickupThing::AbstractNonePickupThing(DOOM::Doom & doom, DOOM::Wad::RawLevel::Thing const & thing, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, thing, height, radius, properties)
{}

DOOM::AbstractNonePickupThing::AbstractNonePickupThing(DOOM::Doom & doom, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, height, radius, properties)
{}

DOOM::AbstractNonePickupThing::~AbstractNonePickupThing() = default;

void	DOOM::AbstractNonePickupThing::pickup(DOOM::Doom & doom, DOOM::AbstractThing & monster)
{}

bool	DOOM::AbstractNonePickupThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Does nothing
  return false;
}