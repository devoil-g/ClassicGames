#include "Doom/Thing/AbstractStaticPickupThing.hpp"

DOOM::AbstractStaticPickupThing::AbstractStaticPickupThing(DOOM::Doom & doom, DOOM::Wad::RawLevel::Thing const & thing, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, thing, height, radius, properties),
  _pickup(false)
{}

DOOM::AbstractStaticPickupThing::AbstractStaticPickupThing(DOOM::Doom & doom, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, height, radius, properties),
  _pickup(false)
{}

DOOM::AbstractStaticPickupThing::~AbstractStaticPickupThing() = default;

void	DOOM::AbstractStaticPickupThing::pickup(DOOM::Doom & doom, DOOM::AbstractThing & monster)
{
  // Set pickup flag
  _pickup = true;
}

bool	DOOM::AbstractStaticPickupThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Return true if thing has already been picked up
  return _pickup;
}